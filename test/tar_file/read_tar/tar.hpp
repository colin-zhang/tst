#ifndef FBC_MESSY_TEST_TAR_HPP_
#define FBC_MESSY_TEST_TAR_HPP_

#include <vector>
#include <string>

/* reference:
	http://www.gnu.org/software/tar/manual/html_node/Standard.html
	http://stackoverflow.com/questions/2505042/how-to-parse-a-tar-file-in-c
	http://directory.fsf.org/wiki/Libtar
	http://work.freenet59.ru/svn/pkgsrc_haiku/trunk/archivers/libarchive/files/contrib/untar.c
	https://codeistry.wordpress.com/2014/08/14/how-to-parse-a-tar-file/
	http://stackoverflow.com/questions/17862383/how-to-know-the-files-inside-the-tar-parser
	https://en.wikipedia.org/wiki/Tar_(computing)
*/

/* tar Header Block, from POSIX 1003.1-1990.  */

// Type flag field
// Value	Meaning
// '0' or (ASCII NUL)	Normal file
// '1'	Hard link
// '2'	Symbolic link
// '3'	Character special
// '4'	Block special
// '5'	Directory
// '6'	FIFO
// '7'	Contiguous file
// 'g'	global extended header with meta data (POSIX.1-2001)
// 'x'	extended header with meta data for the next file in the archive (POSIX.1-2001)
// 'A'–'Z'	Vendor specific extensions (POSIX.1-1988)
// All other values	reserved for future standardization


/* POSIX header.  */

typedef struct posix_header
{                                 /* byte offset */
	char name[100];               /*   0 */  //File name
	char mode[8];                 /* 100 */  //File mode
	char uid[8];                  /* 108 */  //Owner's numeric user ID
	char gid[8];                  /* 116 */  //Group's numeric user ID
	char size[12];                /* 124 */  //File size in bytes (octal base)
	char mtime[12];               /* 136 */  //Last modification time in numeric Unix time format (octal)
	char chksum[8];               /* 148 */  //Checksum for header record
	char typeflag;                /* 156 */  //Type flag
	char linkname[100];           /* 157 */  //Name of linked file
	char magic[6];                /* 257 */  //UStar indicator "ustar" then NUL
	char version[2];              /* 263 */  //UStar version "00"
	char uname[32];               /* 265 */  //Owner user name
	char gname[32];               /* 297 */  //Owner group name
	char devmajor[8];             /* 329 */  //Device major number
	char devminor[8];             /* 337 */  //Device minor number
	char prefix[155];             /* 345 */  //Filename prefix
                                  /* 500 */
} tar_posix_header;

/*
	location  size  field
	0         100   File name
	100       8     File mode
	108       8     Owner's numeric user ID
	116       8     Group's numeric user ID
	124       12    File size in bytes
	136       12    Last modification time in numeric Unix time format
	148       8     Checksum for header block
	156       1     Link indicator (file type)
	157       100   Name of linked file
*/

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"           /* 00 and no null */
#define TVERSLEN 2

/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */

class TarFile {
public:
	TarFile(const char* tar_name);
	bool IsValidTarFile();
	std::vector<std::string> GetFileNames();
	bool GetFileContents(const char* file_name, char* contents);
	size_t GetFileSize(const char* file_name);
	size_t GetTarSize();
	~TarFile();
private:
	FILE* file;
	size_t size;
	std::vector<std::string> file_names;
	std::vector<size_t> file_sizes;
	std::vector<size_t> file_data_start_addrs;
};

int test_tar();

#endif // FBC_MESSY_TEST_TAR_HPP_
