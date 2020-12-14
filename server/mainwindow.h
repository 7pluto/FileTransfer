#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"
#include "debugononroff.h"

namespace Ui { class MainWindow; }
class QListWidgetItem;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString dirPath;
private slots:
    void on_add_clicked();

    void addFilePBSlot();

    void getSendFileList(QString path);
private:
    Ui::MainWindow *ui;
    FILE *fp;
    struct DirInformation{
        char fileName[260];           //文件名字
        char dirName[260];            //目录名字
        unsigned int isDirectory;  //是否是目录
        unsigned int isFile;       //是否是文件
        unsigned int topDir;       //是否是顶层目录
    };
    struct DirInformation dirInfo;
    /*是目录*/
    void ISDir()
    {
        dirInfo.topDir = 0;
        strcpy(dirInfo.fileName," ");
        dirInfo.isDirectory = 1;
        dirInfo.isFile = 0;
    }
    /*是文件*/
    void ISFile()
    {
        dirInfo.topDir = 0;
        strcpy(dirInfo.dirName," ");
        dirInfo.isDirectory = 0;
        dirInfo.isFile = 1;
    }
};
#endif // MAINWINDOW_H
