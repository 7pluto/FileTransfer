#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qdir.h>
#include <qfile.h>
#include <qfiledialog.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    if((fp = fopen("./FileList/FileList.txt","w+"))==NULL)
    {
        qDebug()<<"Clear FILELIST.TXT error!";
    }
    fclose(fp);
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_add_clicked()
{
    addFilePBSlot();
}

void MainWindow::addFilePBSlot()
{
    dirPath = QFileDialog::getExistingDirectory(this, tr("选择文件夹"), "", QFileDialog::DontUseNativeDialog);
    QDir dir(dirPath);
    if(dirPath.isEmpty())
        return;
    this->getSendFileList(dirPath);
    ui->listWidget->addItem(dir.dirName());
}

void MainWindow::getSendFileList(QString path)
{
    QFile filelist("./FileList/FileList.txt");
    QTextStream out(&filelist);
    if(!filelist.open(QFile::WriteOnly|QFile::Text))
    {
        qDebug()<<"write FILELIST.TXT error!"<<endl;
        return;
    }
    QDir dir(path);
    QStringList str;
    str << "*";
    QFileInfoList files = dir.entryInfoList((QStringList)"*",
                                            QDir::Files|QDir::Dirs,QDir::DirsFirst);
    for(int i=0;i<files.count();i++)
    {
        QFileInfo tmpFileInfo = files.at(i);
        QString fileName = tmpFileInfo.absoluteFilePath();

#if defined __DEBUG__
        qDebug() << fileName << "\n";
#endif
        if(fileName=="."||fileName=="..")
            continue;
        if(tmpFileInfo.isFile())
            out << fileName << endl;
        else
            continue;
    }

    filelist.close();
}

