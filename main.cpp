#include "widget.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget mywidget;
    mywidget.setWindowTitle("文件时间修改 V1.6");//设置窗口名称
    mywidget.show();
    mywidget.setFocusPolicy(Qt::StrongFocus);
    
    setbuf(stdout, NULL);//设置printf缓冲为0,使立即输出
    
    if(argc == 2){//若从配置文件打开程序
        QString auto_fileName = QString::fromLocal8Bit(argv[1]);//获取文件路径
        //qDebug()<<auto_fileName<<endl;
        if(auto_fileName.length() >= 2){
            QString fileConvert = "";//转换后的文件名
            for(int i = 0; i < auto_fileName.length(); i++){
                if(auto_fileName[i] == '\\' && i + 1 < auto_fileName.length()){//斜杠转换
                    if(auto_fileName[i + 1] == '\\'){
                        fileConvert += '/';
                        i++;
                    }
                    else fileConvert += auto_fileName[i];
                }
                else fileConvert += auto_fileName[i];
            }
            mywidget.fileNow = fileConvert;
            mywidget.UiGetFileTime(fileConvert);//获取文件时间到UI
        }
    }
    
    a.exec();
    
    return 0;
}
