#ifndef SERVER_H
#define SERVER_H
#include "stdio.h"
#include "winsock2.h"
#include <QDebug>
#include <QProgressBar>
#include "debugononroff.h"
#define SERVERIP "127.0.0.1"
#define SERVERPORT 3333
#define SIZE 4096//4096
#define BUFSIZE 4096

class server
{
public:
    server();
    int RecvFile(SOCKET sock, char * clientIp, u_int clientPort);
    int getDir(SOCKET sock, char * clientIp, u_int clientPort);
    int sendFile(SOCKET sock, char * clientIp, u_int clientPort);
    int startServer();

private:
    int ret,len;
    SOCKET sockListen,sockTX;
    fd_set fdsock;  //保存所有套接字的集合
    fd_set fdread;  //select要检测的可读套接字集合
    FILE *fp;
    struct sockaddr_in addrServer,addrClient;
    QProgressBar *progressBar;
    char recvBuf[BUFSIZE], recvBuf2[BUFSIZE];//定义用于接收客户端发来信息的缓区
    char _ip[10][255], _port[10][255];
    int i,j,count,recvCount=0;
    int msgCount=0;
    int cnums = 0;
};

#endif // SERVER_H
