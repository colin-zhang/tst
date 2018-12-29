#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "sftp_client.h"

using namespace ssh_x;

int main()
{
    int rc;
    SftpClient* sftp_client = new SftpClient();

    rc = sftp_client->connect("192.168.11.24", 22, "root", "colin@123");
    assert(rc == 0);

    sftp_client->openFile("/opt/test.txt", Sftp_FLAG_WRITE|Sftp_FLAG_READ|Sftp_FLAG_CREAT|Sftp_FLAG_TRUNC, 0777);

    char buff[1024];
    sprintf(buff, "%s \n", "hello test! 1234");

    int64_t n = sftp_client->writeFile(buff, strlen(buff));

    printf("n = %ld\n", n);

    FileAttribute arribute;
    sftp_client->getFileAttribute(&arribute);

    printf("filesize = %lu \n", arribute.filesize);

    memset(buff, 0, sizeof buff);
    sftp_client->rewindFile();
    int64_t nn = sftp_client->readFile(buff, sizeof buff);
    printf("nn = %ld, buff = %s\n", nn, buff);

    sftp_client->closeFile();
    delete sftp_client;
}