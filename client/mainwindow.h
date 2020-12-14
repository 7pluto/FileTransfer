#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <qtextedit.h>
#include "client.h"
#include "debugononroff.h"

namespace Ui { class MainWindow; }
class QListWidgetItem;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void GetFileList();
private slots:
    void on_pushButton_clicked();

    void on_upload_clicked();

    void on_download_clicked();

    void on_get_clicked();

private:
    Ui::MainWindow *ui;
    QTextEdit                       textEdit;
    QPushButton pushButton;
    client ct;
    QString re;
};
#endif // MAINWINDOW_H
