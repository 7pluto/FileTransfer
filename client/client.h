#ifndef CLIENT_H
#define CLIENT_H
#include "stdio.h"
#include "stdlib.h"
#include "winsock2.h"
#include <QString>
#include <windows.h>
#include <QDebug>
//#define SERVERIP "127.0.0.1"
#define SERVERPORT 3333
#define SIZE 4096//4096
#define BUFSIZE 4096

class client
{
public:
    client();
    int SendFile(SOCKET sock,QString file_path,QString file_name);
    int getDir(SOCKET);
    int getFile(SOCKET,char filePath[BUFSIZE], char filename[BUFSIZE]);
    int connectServer(QString SERVERIP);
    int ret;
    int len;
    WSADATA data;
    SOCKET sockClient;
    struct sockaddr_in addrServer, addrClient;
    char input[512], clients[512];
    char sendBuf[512],recvBuf[512];

private:
    HANDLE hout;
};

#endif // CLIENT_H
