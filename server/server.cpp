#include "server.h"

#include <QElapsedTimer>
#include <QProgressDialog>
#include <qapplication.h>

char sendBuf[] = "Connect succeed. Please send a message to me.\n"; //发给客户端的信息
char inputs[][10] = {"upload", "dir", "get", "find"};

server::server()
{
    WSADATA wsaData;
    ret=WSAStartup(MAKEWORD(2,2),&wsaData);
    if(SOCKET_ERROR==ret)
    {
        qDebug() << "加载winsock.dll失败！\n";
    }

    sockListen = socket(AF_INET,SOCK_STREAM,0);
    if (INVALID_SOCKET==sockListen)
    {
        qDebug() << "创建套接字失败！\n";
        WSACleanup();
    }

    len = sizeof(struct sockaddr_in);
    memset(&addrServer,0,len);
    addrServer.sin_addr.S_un.S_addr = inet_addr(SERVERIP);
    addrServer.sin_family =AF_INET;
    addrServer.sin_port = htons(SERVERPORT);

    ret=bind(sockListen,( struct sockaddr *)&addrServer,len);
    if(SOCKET_ERROR==ret)
    {
        qDebug() <<"地址绑定失败！\n";
        closesocket(sockListen);
        WSACleanup();
    }

    ret=listen(sockListen,3);
    if(SOCKET_ERROR==ret)
    {
        qDebug() << "listen函数调用失败！\n";
        closesocket(sockListen);
        WSACleanup();
    }
    else
        qDebug() << "服务器启动成功,正在监听......\n";

    FD_ZERO(&fdsock);//初始化 fdsock 为空集合
    FD_SET(sockListen, &fdsock);//将 sockListen 加入到套接字集合 fdsock

}

int server::startServer()
{
    while(true)
    {
        FD_ZERO(&fdread);//初始化 fdread 为空集合
        fdread=fdsock;//将 fdsock 中的所有套接字添加到 fdread 中
        count=fdsock.fd_count;

        if(select(0, &fdread, NULL, NULL, NULL)<0)
        {
            qDebug() <<"Select调用失败！\n";
            break;//终止循环退出程序
        }

        for(i=0;i<count;i++)
        {
            if (FD_ISSET(fdsock.fd_array[i], &fdread))
            {
                if(fdsock.fd_array[i]==sockListen)
                {	//有客户连接请求到达, 接收连接请求
                    len=sizeof(struct sockaddr);
                    sockTX=accept (sockListen, (struct sockaddr *) &addrClient, &len);
                    if(sockTX==INVALID_SOCKET)
                    {
                        qDebug() <<"accept函数调用失败！\n";
                        break;
                    }
                    else
                    {
                        qDebug()<<inet_ntoa(addrClient.sin_addr)<<":"<<ntohs(addrClient.sin_port)<<"连接成功";
                        send(sockTX,sendBuf,strlen(sendBuf)+1,0) ;//发送一条信息
                        FD_SET(sockTX, &fdsock);//将新套接字加入fdsock

                        struct sockaddr_in clients;
                        getpeername(sockTX, (struct sockaddr *)&clients, &len);
                        strcpy(_ip[cnums], inet_ntoa(clients.sin_addr));
                        itoa(ntohs(addrClient.sin_port), _port[cnums], 10);
                        strcat(_ip[cnums],":");
                        cnums++;
                    }

                }
                else
                {
                    getpeername(fdsock.fd_array[i],
                        (struct sockaddr *)&addrClient, &len);

                    memset(recvBuf,0,sizeof(recvBuf));
                    ret=recv(fdsock.fd_array[i],recvBuf,sizeof(recvBuf),0);
                    if(ret == 0)
                    {
                        continue;
                    }
                    if (SOCKET_ERROR==ret)
                    {
                        break;
                    }
#if defined __DEBUG__
                    qDebug() <<"收到命令" << recvBuf;
#endif
                    if(strcmp(recvBuf, inputs[0]) == 0)
                    {
                        RecvFile(fdsock.fd_array[i], inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));
                    }
                    else if(strcmp(recvBuf, inputs[1]) == 0)
                    {
                        getDir(fdsock.fd_array[i], inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));
                    }
                    else if(strcmp(recvBuf, inputs[2]) == 0)
                    {
                        sendFile(fdsock.fd_array[i], inet_ntoa(addrClient.sin_addr), ntohs(addrClient.sin_port));
                    }
                    else if(strcmp(recvBuf, inputs[3]) == 0)
                    {
                        int tmp = 0;
                        while(tmp < cnums && _ip[i])
                        {
                            strcat(_ip[tmp],_port[tmp]);
                            ret = send(fdsock.fd_array[i],_ip[tmp],SIZE,0);
                            if(ret==SOCKET_ERROR){printf("发送失败\n");}
                            tmp++;
                         }
                        printf("发送结束quit\n\n");
                        send(fdsock.fd_array[i], "quit", SIZE, 0);
                    }
                }
            }
        }
    }
    return 0;
}

int server::RecvFile(SOCKET sock, char * clientIp, u_int  clientPort)
{
    int ret;
    FILE *fp;
    char sendBuf[BUFSIZE],recvBuf[BUFSIZE],filename[512];


    memset(recvBuf,0,sizeof(recvBuf));
    ret=recv(sock,recvBuf,sizeof(recvBuf),0);
    if (SOCKET_ERROR==ret)
    {
        return -1;
    }
    if(strlen(recvBuf) == 0)
    {
        return 0;
    }
#if defined __DEBUG__
    qDebug()<<"接到客户端"<<clientIp<<":"<<clientPort<<"发来的文件名:"<<recvBuf;
#endif

    memset(sendBuf,0,sizeof(sendBuf));
    strcpy(sendBuf,recvBuf);
    ret=send(sock,sendBuf,strlen(sendBuf),0);
    if (SOCKET_ERROR==ret)
    {
        return -2;
    }

    char cc[225];
    strcpy(cc,"./clientFile/");
    strcpy(filename,recvBuf);
    fp=fopen(strcat(cc,filename),"wb");
    if (NULL==fp)
    {
        return -3;
        qDebug() << "打开文件错误！";
    }

    memset(recvBuf,0,sizeof(recvBuf));
    ret=recv(sock,recvBuf,sizeof(recvBuf),0);
    if (SOCKET_ERROR==ret)
    {
        return -4;
    }

    int filelen;
    filelen = atoi(recvBuf);


    memset(sendBuf,0,sizeof(sendBuf));
    strcpy(sendBuf,recvBuf);
    ret=send(sock,sendBuf,strlen(sendBuf),0);
    if (SOCKET_ERROR==ret)
    {
        printf("send len error\n");
        return -5;
    }

    int size=0;  //接收到的数据长度
    do
    {
        size=recv(sock,recvBuf,sizeof(recvBuf),0);
        fwrite(recvBuf,size,1,fp);
        filelen-=size;
    }while(size!=0 && filelen>0);

    fclose(fp);
    qDebug() << "成功接收文件";
    return 0;
}

int server::getDir(SOCKET sock, char * clientIp, u_int  clientPort)
{
    int iResult;
    FILE *file;
    char buf[1024];
    int len=0,i=0;
    char *array[1024];

    file=fopen("./FileList/FileList.txt","r");
    if(!file) return -1;
    while(fgets(buf,1024,file))//读取TXT中字符
    {
        len=strlen(buf);
        array[i]=(char*)malloc(len+1);
        if(!array[i])break;
        strcpy(array[i++],buf);
    }

    fclose(file);
    i--;
    while(i>=0&&array[i])
    {
#if defined __DEBUG__
        qDebug() << array[i];//打印test文档的字符
#endif
        iResult = send(sock,array[i],SIZE,0);
        if(iResult==SOCKET_ERROR){printf("发送失败\n");}
        free(array[i--]);
     }

    qDebug() << "发送结束quit";
    send(sock,"quit", SIZE,0);
}

int server::sendFile(SOCKET sock, char * clientIp, u_int clientPort)
{
    int ret,cnt,len1,lenlast;
    char sendBuf[BUFSIZE],recvBuf[BUFSIZE];
    FILE *fp;
    int totlen;
    char filename[512];

    memset(sendBuf,0,sizeof(sendBuf));
    strcpy(sendBuf,"filename");
    ret=send(sock,sendBuf,strlen(sendBuf)+1,0);
    if (SOCKET_ERROR==ret)
    {
        qDebug() << "send ok error\n";
        return -2;
    }

    memset(recvBuf,0,sizeof(recvBuf));
    ret=recv(sock,recvBuf,sizeof(recvBuf),0);
    strcpy(filename, recvBuf);
    if (SOCKET_ERROR==ret )
    {
        qDebug() << "recv filename error\n";
        return -3;
    }
#if defined __DEBUG__
    qDebug() <<filename;
#endif

    fp=fopen(filename,"rb");
    if (NULL==fp)
    {
        qDebug() << "open file error\n";
        return -1;
    }

    fseek(fp,0,SEEK_END);
    totlen=ftell(fp);

    memset(sendBuf,0,sizeof(sendBuf));
    strcpy(sendBuf,filename);
    ret=send(sock,sendBuf,strlen(sendBuf)+1,0);
    if (SOCKET_ERROR==ret)
    {
        qDebug() << "send filename error\n";
        return -2;
    }

#if defined __DEBUG__
    qDebug() << "向服务器发送文件名成功\n";
#endif

    memset(sendBuf,0,sizeof(sendBuf));
    itoa(totlen,sendBuf,10);
    ret=send(sock,sendBuf,strlen(sendBuf)+1,0);
    if (SOCKET_ERROR==ret)
    {
        qDebug() << "send len error\n";
        return -4;
    }

    memset(recvBuf,0,sizeof(recvBuf));
    ret=recv(sock,recvBuf,sizeof(recvBuf),0);
    if (SOCKET_ERROR==ret )
    {
        qDebug() << "recv len error\n";
        return -5;
    }
    cnt=totlen/SIZE;
    len1=SIZE;
    rewind(fp);
    if(totlen%SIZE)
    {
        lenlast=totlen-cnt*SIZE;
        cnt=cnt+1;
    }

    int temp = 0;
    do
    {
        temp+=SIZE;
        fread(sendBuf,SIZE,1,fp);
        ret=send(sock,sendBuf,SIZE,0);
        if (SOCKET_ERROR==ret)
        {
             printf("send buff error\n");
             return -6;
        }
    } while(temp<totlen);

    qDebug() << "发送成功!";
    fclose(fp);

    return 0;
}
