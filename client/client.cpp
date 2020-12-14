#include "client.h"

#include <QProgressDialog>
#include "debugononroff.h"
#include <qfile.h>

client::client()
{

}

int client::connectServer(QString SERVERIP)
{
    ret=WSAStartup(MAKEWORD(2,2),&data);
    if (SOCKET_ERROR==ret)
    {
        return -1;
    }

    sockClient=socket(AF_INET,SOCK_STREAM,0);
    if (INVALID_SOCKET==sockClient)
    {
        WSACleanup();
        return -2;
    }
    char*  SERVERIP_ch;
    QByteArray ba = SERVERIP.toLatin1(); // must
    SERVERIP_ch=ba.data();
#if defined __DEBUG__
    qDebug() << "SERVERIP_ch:"<< SERVERIP_ch;
#endif
    memset(&addrServer,0,sizeof(struct sockaddr_in));
    addrServer.sin_family=AF_INET;
    addrServer.sin_addr.S_un.S_addr=inet_addr(SERVERIP_ch);
    addrServer.sin_port=htons(SERVERPORT);

    len=sizeof(struct sockaddr);
    ret=connect(sockClient,(sockaddr*)&addrServer,len);
    if (SOCKET_ERROR==ret)
    {
        closesocket(sockClient);
        WSACleanup();
        return -3;
    }

    len=sizeof(struct sockaddr);
    ret=getsockname(sockClient,(struct sockaddr*)&addrClient,&len);
    if(SOCKET_ERROR!=ret)
    {
#if defined __DEBUG__
        qDebug() << "连接服务器成功,本地地址为:" << inet_ntoa(addrClient.sin_addr),ntohs(addrClient.sin_port);
#endif
    }

    memset(recvBuf,0,sizeof(recvBuf));
    ret=recv(sockClient,recvBuf,sizeof(recvBuf),0);
    if(SOCKET_ERROR==ret)
    {
#if defined __DEBUG__
        qDebug() << "接收信息失败！错误代码",WSAGetLastError();
#endif
        closesocket(sockClient);
        WSACleanup();
        return -4;
    }
    else
#if defined __DEBUG__
        qDebug() << "收到服务器发来的信息:" << recvBuf;
#endif
    return 0;
}

int client::SendFile(SOCKET sock,QString file_path,QString file_name)
{
    int ret,cnt,len1,lenlast;
    char sendBuf[BUFSIZE],recvBuf[BUFSIZE];
    FILE *fp;
    int totlen;
    char filename[512], filetemp[512], cc[10];
    QByteArray t1 = file_path.toLatin1(), t2 = file_name.toLatin1();
    char *file_path_c = t1.data(), *file_name_c = t2.data();

    strcpy(cc,"/");
    strcpy(filetemp, file_path_c);
    strcpy(filename,file_name_c);
    strcat(filetemp,cc);
#if defined __DEBUG__
    qDebug() <<filetemp << filename;
#endif
    fp=fopen(strcat(filetemp,filename),"rb");
    if (NULL==fp)
    {
        qDebug() << "open file error\n";
        return -1;
    }

    fseek(fp,0,SEEK_END);
    totlen=ftell(fp);
#if defined __DEBUG__
    qDebug() << filename << "长度为:" << totlen;
#endif

    memset(sendBuf,0,sizeof(sendBuf));
    strcpy(sendBuf,filename);

    ret=send(sock,sendBuf,strlen(sendBuf)+1,0);
    if (SOCKET_ERROR==ret)
    {
#if defined __DEBUG__
        qDebug() << "send filename error\n";
#endif
        return -2;
    }

    memset(recvBuf,0,sizeof(recvBuf));
    ret=recv(sock,recvBuf,sizeof(recvBuf),0);
    if (SOCKET_ERROR==ret|| (strcmp(sendBuf,recvBuf)!=0) )
    {
#if defined __DEBUG__
        qDebug() << "recv filename error\n";
#endif
        return -3;
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
    if (SOCKET_ERROR==ret)
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
    QProgressDialog *progressDialog = new QProgressDialog();
    do
    {
        temp+=SIZE;
        fread(sendBuf,SIZE,1,fp);
        ret=send(sock,sendBuf,SIZE,0);
        if (SOCKET_ERROR==ret)
        {
             qDebug() << "send buff error\n";
             return -6;
        }
        QFont font("ZYSong18030", 12);
        progressDialog->setFont(font);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setMinimumDuration(5);
        progressDialog->setWindowTitle("Please Wait");
        progressDialog->setLabelText("transmiting...");
        progressDialog->setRange(0, totlen);
        progressDialog->setValue(temp);
    }while(temp<totlen);
    delete progressDialog;
    qDebug() << "send access!";
    fclose(fp);

    return 0;
}

int client::getDir(SOCKET ConnectSocket)
{
    QFile filelist("./FileList/FileList.txt");
    QTextStream out(&filelist);
    if(!filelist.open(QFile::WriteOnly|QFile::Text))
    {
        qDebug()<<"write FILELIST.TXT error!"<<endl;
        return -1;
    }
    int iResult;
    char recvBuf[BUFSIZE];
    memset(recvBuf,0,sizeof(recvBuf));
    while(true)
    {
        iResult = recv(ConnectSocket,recvBuf,SIZE,0);
        if(strcmp(recvBuf,"quit")==0){break;}
        else{
#if defined __DEBUG__
            qDebug() << recvBuf;
#endif
            out << recvBuf << endl;
        }

        if(SOCKET_ERROR == iResult)
        {
            qDebug() << "recvieve failed with error";
            closesocket(ConnectSocket);
            WSACleanup();
            return -1;
        }
    }
    qDebug() << "getDir succeed!";
}

int client::getFile(SOCKET sock, char filePath[BUFSIZE], char filename[BUFSIZE])
{

    int ret;
    FILE *fp;
    char sendBuf[BUFSIZE],recvBuf[BUFSIZE], temp[BUFSIZE];

    memset(recvBuf,0,sizeof(recvBuf));
    ret=recv(sock,recvBuf,sizeof(recvBuf),0);
    if (SOCKET_ERROR==ret)
    {
        return -1;
        qDebug() << "recv ok error";
    }

    strcpy(temp, "./downloadFile/");
    fp=fopen(strcat(temp,filename),"wb");
    if (NULL==fp)
    {
        return -2;
    }

    memset(sendBuf,0,sizeof(sendBuf));
    strcpy(sendBuf,filePath);
    ret=send(sock,sendBuf,strlen(sendBuf),0);
    if (SOCKET_ERROR==ret)
    {
        return -3;
        qDebug() << "send filename error";
    }

    memset(recvBuf,0,sizeof(recvBuf));
    ret=recv(sock,recvBuf,sizeof(recvBuf),0);
    if (SOCKET_ERROR==ret)
    {
        return -4;
        qDebug() << "recv filename error";
    }


    memset(recvBuf,0,sizeof(recvBuf));
    ret=recv(sock,recvBuf,sizeof(recvBuf),0);
    if (SOCKET_ERROR==ret)
    {
        return -5;
        qDebug() << "recv len error";
    }

    int filelen;
    filelen = atoi(recvBuf);

    memset(sendBuf,0,sizeof(sendBuf));
    strcpy(sendBuf,recvBuf);
    ret=send(sock,sendBuf,strlen(sendBuf),0);
    if (SOCKET_ERROR==ret)
    {
        qDebug() << "send len error\n";
        return -6;
    }

    QProgressDialog *progressDialog = new QProgressDialog();
    int size=0;
    int tt = filelen;
    do
    {
        size=recv(sock,recvBuf,sizeof(recvBuf),0);
        fwrite(recvBuf,size,1,fp);
        filelen-=size;
        QFont font("ZYSong18030", 12);
        progressDialog->setFont(font);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setMinimumDuration(5);
        progressDialog->setWindowTitle("Please Wait");
        progressDialog->setLabelText("transmiting...");
        progressDialog->setRange(0, tt);
        progressDialog->setValue(tt - filelen);
    }while(size!=0 && filelen>0);

    delete progressDialog;
    fclose(fp);
    qDebug() << "getFile access!";

    return 0;
}
