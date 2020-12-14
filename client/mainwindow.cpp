#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->setText("127.0.0.1");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString addr;
    addr = ui->textEdit->toPlainText();
    ct.connectServer(addr);
    ui->pushButton->setText("已连接");
}

void MainWindow::on_upload_clicked()
{
    QString file_name,file_path;
    QString file = QFileDialog::getOpenFileName(this,tr(""),"",tr("")); //选择路径
    QFileInfo fi = QFileInfo(file);
    file_name = fi.fileName();  //获取文件名
    file_path = fi.absolutePath(); //获取文件路径(不包含文件名)
    ct.ret=send(ct.sockClient,"upload",strlen("upload")+1,0);
    if (SOCKET_ERROR==ct.ret)
    {
        printf("send error\n");
        closesocket(ct.sockClient);
        WSACleanup();
    }
    ct.SendFile(ct.sockClient,file_path,file_name);
}

void MainWindow::on_download_clicked()
{
    QString downloadfile = ui->listWidget->currentItem()->text();
    ct.ret=send(ct.sockClient,"get",strlen("get")+1,0);
    if (SOCKET_ERROR==ct.ret)
    {
        printf("send error\n");
        closesocket(ct.sockClient);
        WSACleanup();
    }

    char * filename;
    QByteArray ba = downloadfile.toLatin1();
    filename=ba.data();
    downloadfile = re + "/" + downloadfile;

#if defined __DEBUG__
    qDebug() << "downloadfile" << downloadfile;
#endif

    char * filePath;
    ba = downloadfile.toLatin1();
    filePath=ba.data();

    ct.getFile(ct.sockClient, filePath, filename);
}

void MainWindow::on_get_clicked()
{
    ct.ret=send(ct.sockClient,"dir",strlen("dir")+1,0);
    if (SOCKET_ERROR==ct.ret)
    {
        printf("send error\n");
        closesocket(ct.sockClient);
        WSACleanup();
    }

    ct.getDir(ct.sockClient);
    QFile filelist("./FileList/FILELIST.TXT");
    if(!filelist.open(QFile::ReadOnly|QFile::Text))
    {
        qDebug()<<"open filelist.txt error!"<<endl;
    }

    QTextStream input(&filelist);
    QString line;
    ui->listWidget->clear();            //每同步之前需要先进行清除工作
    while(input.readLineInto(&line))    //把所有文件名进行显示
    {
        QString t;
        if(line == "") continue;
        t = line.section('/',-1);
        re = line.section('/',0,-2);
#if defined __DEBUG__
        qDebug() << "re:" << re;
#endif
        ui->listWidget->addItem(t);
    }
}
