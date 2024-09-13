#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>//这个必须放前面
#include "windows.h"
#include <iostream>

#include <qdebug.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
    QString fileFilter = "";//文件过滤
    QString fileNow = "";//现在打开的文件
    QString folderNow = "";//现在的文件夹
    QString instructionStr = "";//说明信息
    QString instructionStrM = "";//说明信息
    
public:
    QDateTime Filetime2QDatetime(FILETIME ft);//文件时间转QT时间
    FILETIME QDatetime2Filetime(QDateTime qdt);//QT时间转文件时间
    uint8_t qGetFileTime(QString path, QDateTime *qdtC, QDateTime *qdtM, QDateTime *qdtA);//获取文件时间
    uint8_t qSetFileTime(QString path, QDateTime *qdtC, QDateTime *qdtM, QDateTime *qdtA);//设置文件时间
    void UiGetFileTime(QString path, uint8_t cs = 0x07);//获取文件时间到UI
    void UiSetFileTime(QString path, uint8_t cs = 0x07);//从UI设置文件时间
    
    void multiFileHandle(int incOrRand);//多文件处理
    
private slots:
    void on_Bt_open_clicked();//打开文件
    
    void on_Bt_set_clicked();//修改时间
    
    void on_Bt_set_Creat_clicked();//修改创建时间
    void on_Bt_set_Mod_clicked();//修改修改时间
    void on_Bt_set_Access_clicked();//修改访问时间
    
    void on_Bt_copy_Creat_clicked();//复制创建时间
    void on_Bt_copy_Mod_clicked();//复制修改时间
    void on_Bt_copy_Access_clicked();//复制访问时间
    
    void on_Bt_set_multiFileInc_clicked();//多文件时间递进
    void on_Bt_set_multiFileRand_clicked();//多文件随机波动
    
    void on_Bt_set_instruction_clicked();//说明
    void on_Bt_set_instructionM_clicked();//多文件说明
    
private:
    Ui::Widget *ui;
};

#endif // WIDGET_H










