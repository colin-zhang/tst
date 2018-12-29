// 2017-05
#include "sftp_client.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>

#include <iostream>

namespace ssh_x
{

SshSession::SshSession(const char* host, uint16_t port, long timeout)
{
    m_session = libssh2_session_init();
    assert(m_session != NULL);
    libssh2_session_set_timeout(m_session, timeout);
    //pthread_mutex_init(&m_lock, NULL);
    m_host.assign(host);
    m_port = port;
    m_sock = -1;
}

SshSession::~SshSession()
{
    if (m_session) {
        libssh2_session_disconnect(m_session, "Bye");
        libssh2_session_free(m_session);
        ::close(m_sock);
        //pthread_mutex_destroy(&m_lock);
        //std::cout << "~SshSession" << std::endl;
    }
}

int SshSession::connect()
{
    int rc;
    struct addrinfo ai_hints, *result;
    int protoFmly = AF_INET;
    if(m_host.find(':') != std::string::npos) {
        protoFmly = AF_INET6;
    }
    m_sock = ::socket(protoFmly, SOCK_STREAM, 0);
    if (m_sock < 0) {
        std::cerr << "socket error," << strerror(errno) << std::endl;
        return -1;
    }
    memset(&ai_hints, 0, sizeof(struct addrinfo));
    ai_hints.ai_family   = AF_UNSPEC;
    ai_hints.ai_socktype = SOCK_STREAM;
    ai_hints.ai_protocol = IPPROTO_TCP;

    char port_str[32];
    snprintf(port_str, sizeof port_str, "%d", m_port);

    rc = getaddrinfo(m_host.c_str(), port_str, &ai_hints, &result);
    if (rc || !result) {
        ::close(m_sock);
        return -1;
    }

    rc = ::connect(m_sock, result->ai_addr, result->ai_addrlen);
    if (rc < 0) {
        ::close(m_sock);
        std::cerr << "connect error " << strerror(errno) << std::endl;
        return -1;
    }
    freeaddrinfo(result);

    if (rc < 0) {
        ::close(m_sock);
        return -1;
    }
    rc = libssh2_session_handshake(m_session, m_sock);
    if (0 != rc) {
        ::close(m_sock);
        std::cerr << "Failure establishing SSH session: " << rc << std::endl;
        return -1;
    }
    return 0;
}

int SshSession::auth(const char* user, const char* passwd)
{
    if (libssh2_userauth_password(m_session, user, passwd) != 0) {
        //lastError();
        return -1;
    }
    return 0;
}

int SshSession::getSessionError(char** errmsg)
{
    int errlen;
    if (NULL != errmsg) {
        libssh2_session_last_error(m_session, errmsg, &errlen, 0);
    }
    return libssh2_session_last_errno(m_session);
}

int SshSession::getSessionError()
{
    return libssh2_session_last_errno(m_session);
}

void SshSession::setBlocking(bool blocking)
{
    libssh2_session_set_blocking(m_session, blocking);
}

LIBSSH2_SFTP* SshSession::startSftpSession()
{
    LIBSSH2_SFTP* sftp_s = libssh2_sftp_init(m_session);
    return sftp_s;
}

SftpClient::SftpClient()
{
    m_sftp_session = NULL;
    m_sftp_handle = NULL;
}

SftpClient::~SftpClient()
{
    if (m_ssh_session != NULL) {
        libssh2_sftp_shutdown(m_sftp_session);
        delete m_ssh_session;
    }
}

int SftpClient::connect(const char* host, uint16_t port, const char* user, const char* passwd)
{
    int rc;
    m_ssh_session = new SshSession(host, port, kTimeOut);
    if (NULL == m_ssh_session) {
        //TODO
        return -1;
    }
    rc = m_ssh_session->connect();
    if (rc < 0) {
        //TODO, error occur
        return -1;
    }

    rc = m_ssh_session->auth(user, passwd);
    if (rc < 0) {
        //TODO
        return -1;
    }

    m_sftp_session = m_ssh_session->startSftpSession();
    if (NULL == m_ssh_session) {
        //TOOD
        return -1;
    }
    return 0;
}

unsigned long SftpClient::getError()
{
    return libssh2_sftp_last_error(m_sftp_session);
}

int SftpClient::openFile(const char *path, unsigned long flags, long mode)
{
    if (m_sftp_handle != NULL) {
        libssh2_sftp_close_handle(m_sftp_handle);
        m_sftp_handle = NULL;
    }

    m_sftp_handle = libssh2_sftp_open(m_sftp_session, path, flags, mode);
    if (NULL == m_sftp_handle) {
        return -1;
    }
    return 0;
}

int SftpClient::getFileAttribute(FileAttribute* fattri)
{
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    int rc = libssh2_sftp_fstat(m_sftp_handle, &attrs);
    if (rc < 0) {
        return -1;
    }
    fattri->flags = attrs.flags;
    fattri->filesize = attrs.filesize;
    fattri->uid = attrs.uid;
    fattri->gid = attrs.gid;
    fattri->permissions = attrs.permissions;
    fattri->atime = attrs.atime;
    fattri->mtime = attrs.mtime;
    return 0;
}

int SftpClient::closeFile()
{
    return libssh2_sftp_close(m_sftp_handle);
}

ssize_t SftpClient::readFile(char* buffer, size_t buffer_maxlen)
{
    return libssh2_sftp_read(m_sftp_handle, buffer, buffer_maxlen); 
}

ssize_t SftpClient::writeFile(const char* buffer, size_t count)
{
    ssize_t rc = libssh2_sftp_write(m_sftp_handle, buffer, count);
    return rc;
}

void SftpClient::rewindFile()
{
    libssh2_sftp_seek64(m_sftp_handle, 0);
}

void SftpClient::fileSync()
{
    libssh2_sftp_fsync(m_sftp_handle);
}

int SftpClient::mkdir(const char* path, long mode)
{
    return libssh2_sftp_mkdir(m_sftp_session, path, mode);
}

int SftpClient::rename(const char* source, const char* destination)
{
    return libssh2_sftp_rename(m_sftp_session, source, destination);
}


int SftpClient::upload(const char* local, const char* remote)
{
    //TODO
    LIBSSH2_SFTP_HANDLE* sftp_handle = libssh2_sftp_open(m_sftp_session, remote, Sftp_FLAG_WRITE|Sftp_FLAG_CREAT|Sftp_FLAG_TRUNC, 0777);
    if (NULL == sftp_handle) {
        return -1;
    }

    FILE* f_local = fopen(local, "rb");
    if (NULL == f_local) {
        return -1;
    }

    size_t nread = 0;
    char mem[4096*4] = {0};
    char* ptr = NULL;
    int rc = 0;

    do {
        nread = fread(mem, 1, sizeof(mem), f_local);
        if (nread <= 0) {
            /* end of file */ 
            break;
        }
        ptr = mem;
        do {
            /* write data in a loop until we block */ 
                rc = libssh2_sftp_write(sftp_handle, ptr, nread);
                if(rc < 0) {
                    break;
                }
                ptr += rc;
                nread -= rc;
        } while (nread);
 
    } while (rc > 0);


    fclose(f_local);
    libssh2_sftp_close(sftp_handle);

    if (getError() != 0) {
        return -1;
    }
    return 0;
}

int SftpClient::download(const char* remote, const char* local)
{
    //TODO
    FILE* f_local = fopen(local, "wb");
    if (NULL == f_local) {
        return -1;
    }

    LIBSSH2_SFTP_HANDLE* sftp_handle = libssh2_sftp_open(m_sftp_session, remote, Sftp_FLAG_READ, 0777);
    if (NULL == sftp_handle) {
        fclose(f_local);
        return -1;
    }

    char mem[4096*4] = {0};
    int rc = 0;

    do {
        memset(mem, 0, sizeof(mem));
        rc = libssh2_sftp_read(sftp_handle, mem, sizeof(mem));
        if (rc > 0) {
            fwrite(mem, 1, rc, f_local);
        } else {
            break;
        }
    } while (1);


    fclose(f_local);
    libssh2_sftp_close(sftp_handle);

    if (getError() != 0) {
        return -1;
    }
    return 0;
}


} //end of namespace uim
