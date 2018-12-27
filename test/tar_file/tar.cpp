#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

#include "sample/tar.h"
//https://en.wikipedia.org/wiki/Tar_(computing)
// Type flag field
// Value    Meaning
// '0' or (ASCII NUL)   Normal file
// '1'  Hard link
// '2'  Symbolic link
// '3'  Character special
// '4'  Block special
// '5'  Directory
// '6'  FIFO
// '7'  Contiguous file
// 'g'  global extended header with meta data (POSIX.1-2001)
// 'x'  extended header with meta data for the next file in the archive (POSIX.1-2001)
// 'A'–'Z'  Vendor specific extensions (POSIX.1-1988)
// All other values reserved for future standardization

// enum UStarTypeFlag
// {
//     KTypeFlagNormalFile = 0x30,
//     KTypeFlagNormalFile = 0x31,
//     KTypeFlagNormalFile = 0x32,
//     KTypeFlagNormalFile = 0x33,
//     KTypeFlagNormalFile = 0x34,
//     KTypeFlagNormalFile = 0x35,
// };

struct UStarHeader
{                                              
    char data[0];                  /* byte offset */
    char name[100];              /*  0    */  //File name
    char mode[8];                  /* 100 */  //File mode
    char uid[8];                    /* 108 */  //Owner's numeric user ID
    char gid[8];                    /* 116 */  //Group's numeric user ID
    char size[12];                /* 124 */  //File size in bytes (octal base)
    char mtime[12];              /* 136 */  //Last modification time in numeric Unix time format (octal)
    char chksum[8];              /* 148 */  //Checksum for header record
    char typeflag;                /* 156 */  //Type flag
    char linkname[100];      /* 157 */  //Name of linked file
    char magic[6];                /* 257 */  //UStar indicator "ustar" then NUL
    char version[2];            /* 263 */  //UStar version "00"
    char uname[32];              /* 265 */  //Owner user name
    char gname[32];              /* 297 */  //Owner group name
    char devmajor[8];          /* 329 */  //Device major number
    char devminor[8];          /* 337 */  //Device minor number
    char prefix[155];          /* 345 */  //Filename prefix
    char padding[12];          /* 500 */
};

static void DecToOct(size_t value, char* where, size_t size)
{
    unsigned int  v = value;
    size_t i = size - 1;
    do
    {
          where[--i] = '0' + (v & ((1 << 3) - 1));
          v >>= 3;
    } while (i);
    //snprintf(where, size, "0%lo", value);
}

Tar::Tar()
    : root_("")
    , gzip_(false)
    , root_dir_(false)
    , header_(NULL)
    , dst_(NULL)
{
    header_ = new UStarHeader;
    InitHeader(header_);

}

Tar::~Tar()
{
    delete header_;
    delete[] dst_;
}

void Tar::Set(const char* root, int compress_level, size_t max_size)
{
    if (strlen(root) > 0) {
        root_ = std::string(root);
        root_dir_ = true;
    }
    if (compress_level >= 0 || compress_level <= 8) {
        compress_level_ =  compress_level;
        gzip_ = true;
    }

    if (max_size > 0) {
        max_size_ = max_size;
        data_.reserve(max_size_ + (1 << 20));
        if (dst_ != NULL) {
            delete[] dst_;
        }
        dst_ = new Bytef[max_size_];
    }
}

int Tar::InitHeader(UStarHeader* hp)
{
    struct group * group;
    struct passwd * passwd;
    passwd = getpwuid(getuid());
    group = getgrgid(getgid());
    memset(hp, 0, sizeof(*hp));
    strcpy(hp->version, "00");
    strcpy(hp->magic, "ustar");
    strcpy(hp->gname, group->gr_name);
    strcpy(hp->uname, passwd->pw_name);
    DecToOct(getuid(), hp->uid, sizeof(hp->uid));
    DecToOct(getgid(), hp->uid, sizeof(hp->gid));
    DecToOct(0, hp->devmajor, sizeof(hp->devmajor));
    DecToOct(0, hp->devminor, sizeof(hp->devminor));
    DecToOct(33279, hp->mode, sizeof(hp->devminor));
    return 0;
}

void Tar::HeaderChecksum(UStarHeader* hp)
{
    int i = 0;
    size_t sum = 0;
    char *p = NULL;
    p = hp->data;
    memset(hp->chksum, 0x20, sizeof(hp->chksum));
    for (i = sizeof(*hp); i-- != 0; ) {
        sum += 0xFF & *p++;
    }
    hp->chksum[6] = '\0';  
    DecToOct(sum, hp->chksum, sizeof(hp->chksum) - 1);
}

int Tar::AddFilePseudo(const char* file_name, size_t file_size)
{
    return AddFilePseudo(file_name, file_size, '0');
}

int Tar::AddFilePseudo(const char *file_name, size_t file_size, char type_flag)
{
    if (root_dir_) {
        snprintf(header_->name, sizeof(header_->name), "%s/%s", root_.c_str(), file_name);
    } else {
        snprintf(header_->name, sizeof(header_->name), "%s", file_name);
    }
    header_->typeflag = type_flag;
    DecToOct(file_size, header_->size, sizeof(header_->size));
    DecToOct(time(NULL), header_->mtime, sizeof(header_->mtime));
    HeaderChecksum(header_);
    return 0;
}

int Tar::AddFile(const char* file_name, const char* data, size_t file_size)
{
    size_t pad_len = 512 - file_size % 512;
    pad_len = pad_len % 512;
    AddFilePseudo(file_name, file_size);

    data_.insert(data_.end(), header_->data, header_->data + 512);
    data_.insert(data_.end(), data, data + file_size);
    data_.insert(data_.end(), pad_len, 0x00);
    
    return 0;
}

int Tar::AddFileHeader(const char* file_name, size_t file_size)
{
    AddFilePseudo(file_name, file_size);
    data_.insert(data_.end(), header_->data, header_->data + 512);
    append_cnt_ = 0;
    return 0;
}

int Tar::AppendFileData(const char *data, size_t data_len)
{
    if (data_len == 0 ) {
        return 0;
    }
    data_.insert(data_.end(), data, data + data_len);
    append_cnt_ += data_len;
    return 0;
}

int Tar::FinishFileData(const char *data, size_t data_len)
{
    AppendFileData(data, data_len);
    size_t pad_len = 512 - append_cnt_ % 512;
    pad_len = pad_len % 512;
    data_.insert(data_.end(), pad_len, 0x00);
    append_cnt_ = 0;
    return 0;
}

int Tar::AddFile(const char* path)
{
    struct stat buf;
    if (stat(path, &buf) != 0)
    {
        printf("%s\n", "stat");
        return -1;
    }
    size_t size = buf.st_size;
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        printf("%s\n", "fopen");
        return -1;
    }
    char* p = new char[size];
    fread(p, 1, size, fp);
    AddFile(path, p, size);
    delete[] p;
    fclose(fp);
    return 0;
}

int Tar::DumpFile(const char* path)
{
    int ret = 0;
    std::string file = std::string(path);
    std::string file_tmp;

    if (gzip_) {
        file += ".tar.gz";
        file_tmp = file + ".tmp";
        uLongf dest_length = data_.size() + (4 << 20);
        ret = GzipCompress(dst_, &dest_length, (Bytef*)&data_.front(), data_.size());
        if (ret != 0) 
        {
            printf("%s\n", "GzipCompress error !!");
        } else {
            FILE *fp = fopen(file_tmp.c_str(), "wb");
            if (fp)
            {
                fwrite(dst_, 1, dest_length, fp);
                fclose(fp);
                rename(file_tmp.c_str(), file.c_str());
            } else {
                printf("fail to open %s, error=%s\n", file_tmp.c_str(),  strerror(errno));
                ret = -1;
            }
        }
    } else {
        file += ".tar";
        file_tmp += ".tar.tmp";
        FILE *fp = fopen(file_tmp.c_str(), "wb");
        if (fp)
        {
            fwrite(&data_.front(), 1, data_.size(), fp);
            fclose(fp);
            rename(file_tmp.c_str(), file.c_str());
        }
    }
    // int fd = open(file.c_str(), O_CREAT | O_WRONLY | O_CLOEXEC | O_TRUNC, 0666);
    // if (fd < 0)
    // {
    //     printf("%s\n", "error !");
    //     return -1;
    // }
    // write(fd, &data_.front(), data_.size());
    // close(fd);
    data_.clear();
    return ret;
}

int Tar::GzipCompress(Bytef* dest, uLongf* dest_length, const Bytef* source, uLong source_length)
{
      z_stream stream;
      stream.next_in = (Bytef*)(source);
      stream.avail_in = static_cast<uInt>(source_length);
      stream.next_out = dest;
      stream.avail_out = static_cast<uInt>(*dest_length);
      if (static_cast<uLong>(stream.avail_out) != *dest_length) {
            return Z_BUF_ERROR;
      }

      stream.zalloc = static_cast<alloc_func>(0);
      stream.zfree = static_cast<free_func>(0);
      stream.opaque = static_cast<voidpf>(0);

      gz_header gzip_header;
      memset(&gzip_header, 0, sizeof(gzip_header));
      int err = deflateInit2(&stream,
                             compress_level_,
                             Z_DEFLATED,
                             MAX_WBITS + kWindowBitsToGetGzipHeader,
                             kZlibMemoryLevel,
                             Z_DEFAULT_STRATEGY);
      if (err != Z_OK) {
            return err;
      }

      err = deflateSetHeader(&stream, &gzip_header);
      if (err != Z_OK) {
            return err;
      }

      err = deflate(&stream, Z_FINISH);
      if (err != Z_STREAM_END) {
            deflateEnd(&stream);
            return err == Z_OK ? Z_BUF_ERROR : err;
      }
      *dest_length = stream.total_out;
      err = deflateEnd(&stream);
      return err;
}

size_t Tar::GetBuffSize()
{
    return data_.size();
}

size_t Tar::GetBuffCap()
{
    return data_.capacity();
}

size_t Tar::GetMaxFileSize()
{
    return max_size_;
}

int Tar::GetCompressLevel()
{
    return compress_level_;
}
