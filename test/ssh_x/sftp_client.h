#ifndef _LIB_SFTP_CLIENT_H_
#define _LIB_SFTP_CLIENT_H_

#include <stdint.h>
#include <memory>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <string>

#define LIB_SFTP_CLIENT_SMARTPTR 0

namespace ssh_x {

#define Sftp_FLAG_READ        0x01
#define Sftp_FLAG_WRITE       0x02
#define Sftp_FLAG_APPEND      0x04
#define Sftp_FLAG_CREAT       0x08
#define Sftp_FLAG_TRUNC       0x10
#define Sftp_FLAG_EXCL        0x20

struct FileAttribute 
{
    /* If flags & ATTR_* bit is set, then the value in this
      * struct will be meaningful Otherwise it should be ignored
      */
     unsigned long flags;
     /* size of file, in bytes */
     uint64_t filesize;
    /* numerical representation of the user and group owner of
      * the file
      */
     unsigned long uid, gid;
    /* bitmask of permissions */
     unsigned long permissions;
    /* access time and modified time of file */
     unsigned long atime, mtime;
};

class SshSession
{
public:
    SshSession(const char* host, uint16_t port, long timeout);
    ~SshSession();
    int connect();
    int auth(const char* user, const char* passwd);
    int getSessionError();
    int getSessionError(char** errmsg);
    void setBlocking(bool blocking);
    LIBSSH2_SFTP* startSftpSession();
private:
    LIBSSH2_SESSION* m_session;
    std::string m_host;
    uint16_t m_port;
    int m_sock;
};

class SftpClient
{
public:
    static const long kTimeOut=10*1000;
public:
    SftpClient();
    ~SftpClient();
    int connect(const char* host, uint16_t port, const char* user, const char* passwd);
    unsigned long getError();
    int openFile(const char* path, unsigned long flags, long mode);
    int getFileAttribute(FileAttribute* fattri);
    int closeFile();
    ssize_t readFile(char* buffer, size_t buffer_maxlen);
    ssize_t writeFile(const char* buffer, size_t count);
    void rewindFile();
    void fileSync();

    int mkdir(const char* path, long mode);
    int rename(const char* source, const char* destination);

    int upload(const char* local, const char* remote);
    int download(const char* remote, const char* local);
    
    // int upload(const char* local);
    // int download(const char* local);
private:
    //TODO c++11 smart ptr
    SshSession* m_ssh_session;
    LIBSSH2_SFTP* m_sftp_session;

#if LIB_SFTP_CLIENT_SMARTPTR
    std::unique_ptr<LIBSSH2_SFTP_HANDLE> m_sftp_handle;
#else
    LIBSSH2_SFTP_HANDLE* m_sftp_handle;
#endif

};


}

#endif  