#include "mainwindow.h"

#include <QApplication>
#include "mythread.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    MyThread myThread;
    myThread.start();
    w.show();
    return a.exec();
}
