#ifndef TAR_FILE_H_
#define TAR_FILE_H_

#include <stdint.h>
#include <string>
#include <vector>

#include "zlib.h"

struct UStarHeader;

class Tar
{
public:
    Tar();
    ~Tar();

    void Set(const char* root, int compress_level, size_t max_size);

    int AddFile(const char* file_name, const char *data, size_t file_size);
    int AddFile(const char* path);

    int AddFileHeader(const char* file_name, size_t file_size);
    int AppendFileData(const char *data, size_t data_len);
    int FinishFileData(const char *data, size_t data_len);

    int DumpFile(const char* path);

    size_t GetBuffSize();
    size_t GetBuffCap();
    size_t GetMaxFileSize();
    int GetCompressLevel();
private:
    int InitHeader(UStarHeader* hp);
    void HeaderChecksum(UStarHeader* hp);
    int AddFilePseudo(const char* file_name, size_t file_size);
    int AddFilePseudo(const char* file_name, size_t file_size, char type_flag);
    int GzipCompress(Bytef* dest, uLongf* dest_length, const Bytef* source, uLong source_length);
private:
    bool gzip_;
    bool gziphelp_;
    bool root_dir_;
    std::string root_;
    std::vector<char> data_;
    Bytef* dst_;
    UStarHeader *header_;
    size_t append_cnt_;
    size_t max_size_;
private:
    int compress_level_;
    static const int kZlibMemoryLevel = 9;
    static const int kWindowBitsToGetGzipHeader = 16;
};

#endif

    
