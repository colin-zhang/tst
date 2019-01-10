#include <iostream>
#include <string>
#include <ctime>
//for protobuf
#include "heartbeat.pb.h"
//for socket
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

using namespace std;
using namespace HeartBeat;

const int BUFFSIZE = 128;
int main() {
    //建立socket
    int socketfd ;
    struct sockaddr_in seraddr ;
    string hostip = "127.0.0.1";
    //链接，尝试3次
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Fail to open the socket!" << endl;
        exit(-1);
    }
    //地址置空
    bzero(&seraddr, sizeof(seraddr));
    seraddr.sin_family = AF_INET ;
    seraddr.sin_port = htons(9999);
    seraddr.sin_addr.s_addr = inet_addr(hostip.c_str());
    //尝试连接到服务端地址
    if (connect(socketfd, (struct sockaddr *)&seraddr, sizeof(seraddr)) < 0) {
        cout << "connect to server failed ..." << endl;
        close(socketfd);
        return -1;
    }

    HeartInfo myprotobuf;
    while (1) {
        int curtime = time(NULL) ;
        //以下方法的实现可以Test.pb.h中找到
        myprotobuf.set_curtime(curtime);
        myprotobuf.set_hostip("127.0.0.1");
        //protobuf的序列化方式之一
        char buff[BUFFSIZE];
        myprotobuf.SerializeToArray(buff, BUFFSIZE);

        if (send(socketfd, buff, strlen(buff), 0) < 0) {
            cout << "errno = " << errno << endl;
            cout << curtime << ": send failed ..." << endl;
            break;
        }
        cout << curtime << ": send success ..." << endl;
        sleep(1); //每隔5s发送一次
    }
    close(socketfd);
    return 0;
}
