#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    
    fileFilter = "所有格式(*.*);;"
                 "图片(*.png *.jpg *.jpeg *.bmp *.webp *.raw);;"
                 "影片(*.mp4 *.avi *.mpeg *.wmv *.flv *.gif);;"
                 "文档(*.txt *.doc *.docx *.pdf *.pptx *.xlsx);;"
                 "程序(*.exe *.dll *.cpp *.c *.h *.py *.java);;"
                 "压缩包(*.rar *.zip *.7z *.tar *.gz)";
    
    instructionStr = 
            "打开文件后左侧会显示读取到的时间信息\n"
            "右侧是要设置的时间,可点下拉设置或直接输入\n"
            "点→可以将左边时间复制到右边\n"
            "点击右边三个修改按键可以单独修改一项时间\n"
            "点击右上的大修改按钮则同时修改三项时间";
    
    instructionStrM = 
            "把修改勾选上对应的项才会被修改\n"
            "时间递进:\n"
            "以设置时间为起点按增减量递进\n"
            "随机波动:\n"
            "以文件当前时间为基准按波动范围波动\n"
            "设置创建时间的年份将决定随机种子\n"
            "可选三项是否同步波动,由设置时间秒数控制\n"
            "将设置时间秒数模3,相同的项则会同步波动\n"
            "可选分布类型,由设置时间分钟数控制\n"
            "为0则为均匀分布,否则为三角形分布\n"
            "可选分布中心,由设置时间小时数控制\n"
            "为0则正负对称,为1仅正分布,否则仅负分布";
    
//    QFont font = this->font();
//    printf("%d\n",font.pointSize());
//    font.setPointSize(7);//默认为9
//    this->setFont(font);
}

Widget::~Widget(){
    delete ui;
}

QDateTime Widget::Filetime2QDatetime(FILETIME ft)//文件时间转QT时间
{
    FILETIME lft;
    FileTimeToLocalFileTime(&ft, &lft);//文件时间转本地文件时间
    SYSTEMTIME st;
    FileTimeToSystemTime(&lft, &st);//本地文件时间转系统时间
    QDateTime qdt;
    qdt.setDate(QDate(st.wYear, st.wMonth, st.wDay));//设置年月日
    qdt.setTime(QTime(st.wHour, st.wMinute, st.wSecond));//设置时分秒
    return qdt;
}

FILETIME Widget::QDatetime2Filetime(QDateTime qdt)//QT时间转文件时间
{
    SYSTEMTIME st;
    st.wYear = qdt.date().year();
    st.wMonth = qdt.date().month();
    st.wDay = qdt.date().day();
    st.wDayOfWeek = qdt.date().dayOfWeek();
    st.wHour = qdt.time().hour();
    st.wMinute = qdt.time().minute();
    st.wSecond = qdt.time().second();
    st.wMilliseconds = qdt.time().msec();
    FILETIME lft;
    SystemTimeToFileTime(&st, &lft);//系统时间转本地文件时间
    FILETIME ft;
    LocalFileTimeToFileTime(&lft, &ft);//本地文件时间转文件时间
    return ft;
}

uint8_t Widget::qGetFileTime(QString path, QDateTime *qdtC, QDateTime *qdtM, QDateTime *qdtA)//获取文件时间
{
    if(path.isEmpty()) return 1;//若路径为空
    QFile file(path);//创建文件对象
    if(!file.exists()) return 2;//若文件不存在
    QFileInfo fileinfo = QFileInfo(path);//文件信息
    if(qdtC) *qdtC = fileinfo.created();//创建时间
    if(qdtM) *qdtM = fileinfo.lastModified();//修改时间
    if(qdtA) *qdtA = fileinfo.lastRead();//访问时间
    return 0;
}

uint8_t Widget::qSetFileTime(QString path, QDateTime *qdtC, QDateTime *qdtM, QDateTime *qdtA)//设置文件时间
{
    if(path.isEmpty()) return 1;//若路径为空
    
    HANDLE hFile;//文件句柄
    QString url = path;//文件路径
    hFile = CreateFileW(url.toStdWString().data(), GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    FILETIME lpCreationTime;//创建时间
    FILETIME lpLastAccessTime;//访问时间
    FILETIME lpLastWriteTime;//修改时间
    if(!GetFileTime(hFile, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime)){//获取时间信息
        return 2;//获取原时间失败
    }
    
    //ui->dateTime_setCreat->setDateTime(Filetime2QDatetime(lpCreationTime));
    if(qdtC){//修改创建时间
        lpCreationTime = QDatetime2Filetime(*qdtC);
    }
    if(qdtM){//修改修改时间
        lpLastWriteTime = QDatetime2Filetime(*qdtM);
    }
    if(qdtA){//修改访问时间
        lpLastAccessTime = QDatetime2Filetime(*qdtA);
    }
    
    if(!SetFileTime(hFile, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime)){//设置时间信息
        return 3;//设置时间失败
    }
    return 0;
}

void Widget::UiGetFileTime(QString path, uint8_t cs)//获取文件时间到UI
{
    QDateTime qdtC, qdtM, qdtA;
    uint8_t resp = qGetFileTime(path, &qdtC, &qdtM, &qdtA);//获取文件时间
    
    if(resp == 1) QMessageBox::critical(this,"提示","无路径");
    else if(resp == 2) QMessageBox::critical(this,"提示","路径不存在");
    if(resp != 0) return;
    
    if(cs & 0x01) ui->dateTime_getCreat->setDateTime(qdtC);//显示创建时间
    if(cs & 0x02) ui->dateTime_getMod->setDateTime(qdtM);//显示修改时间
    if(cs & 0x04) ui->dateTime_getAccess->setDateTime(qdtA);//显示访问时间
}

void Widget::UiSetFileTime(QString path, uint8_t cs)//从UI设置文件时间
{
    QDateTime qdtC = ui->dateTime_setCreat->dateTime();
    QDateTime qdtM = ui->dateTime_setMod->dateTime();
    QDateTime qdtA = ui->dateTime_setAccess->dateTime();
    uint8_t resp = qSetFileTime(path, (cs & 0x01) ? (&qdtC) : NULL, 
                                      (cs & 0x02) ? (&qdtM) : NULL, 
                                      (cs & 0x04) ? (&qdtA) : NULL);//设置文件时间
    
    if(resp == 1) /*QMessageBox::critical(this,"提示","无路径")*/;
    else if(resp == 2) QMessageBox::critical(this,"提示","获取原时间失败");
    else if(resp == 3) QMessageBox::critical(this,"提示","设置时间失败");
    if(resp != 0);
}

void Widget::on_Bt_open_clicked()//打开文件
{
    QString fileNew = QFileDialog::getOpenFileName(this, QStringLiteral("打开文件"),
                                                   folderNow, fileFilter);
    if(fileNew.isEmpty()) return;//为空则返回
    fileNow = fileNew;//记录路径
    QFileInfo fileinfo = QFileInfo(fileNow);//文件信息
    folderNow = fileinfo.path();//记录文件夹
    ui->lineEdit_fileName->setText(fileinfo.filePath());//显示文件路径
    UiGetFileTime(fileNow);//获取文件时间
}

void Widget::multiFileHandle(int incOrRand)//多文件处理
{
    QDateTime qdtC = ui->dateTime_setCreat->dateTime();//设置时间
    QDateTime qdtM = ui->dateTime_setMod->dateTime();
    QDateTime qdtA = ui->dateTime_setAccess->dateTime();
    
    int positiveOnlyC = ui->dateTime_setCreat->time().hour();//小时数为0则为正负对称分布 为1仅正分布 否则仅负分布
    int positiveOnlyM = ui->dateTime_setMod->time().hour();
    int positiveOnlyA = ui->dateTime_setAccess->time().hour();
    int distributionC = ui->dateTime_setCreat->time().minute() > 0;//分钟数为0则为均匀分布 否则为三角分布
    int distributionM = ui->dateTime_setMod->time().minute() > 0;
    int distributionA = ui->dateTime_setAccess->time().minute() > 0;
    int syncC = ui->dateTime_setCreat->time().second() % 3;//秒数决定使用3个随机数中的哪个 即决定三个数的同步与否
    int syncM = ui->dateTime_setMod->time().second() % 3;
    int syncA = ui->dateTime_setAccess->time().second() % 3;
    
    int dTimeC = ui->sBox_rangeC->value();//时间递进或随机波动量
    int dTimeM = ui->sBox_rangeM->value();
    int dTimeA = ui->sBox_rangeA->value();
    bool ifC = ui->cB_ifC->isChecked();//是否修改此项
    bool ifM = ui->cB_ifM->isChecked();
    bool ifA = ui->cB_ifA->isChecked();
    
    srand(ui->dateTime_setCreat->date().year());//使用创建时间年份作为随机种子
    
    
    QStringList files = QFileDialog::getOpenFileNames(this, QStringLiteral("按希望的顺序选择文件"),
                                                      folderNow, fileFilter,
                                                      NULL, QFileDialog::DontUseNativeDialog);
    
    
    for(int i = 0; i < files.count(); i++){//遍历路径列表
        //qDebug() << files[i] << endl;
        uint8_t resp = 0;
        if(incOrRand == 0){//递进
            resp = qSetFileTime(files[i], ifC ? (&qdtC) : NULL, 
                                          ifM ? (&qdtM) : NULL, 
                                          ifA ? (&qdtA) : NULL);//设置文件时间
            qdtC = qdtC.addSecs(dTimeC);//递进
            qdtM = qdtM.addSecs(dTimeM);
            qdtA = qdtA.addSecs(dTimeA);
        }
        else{//随机波动
            resp = qGetFileTime(files[i], &qdtC, &qdtM, &qdtA);//获取文件时间
            double randNum[6];
            for(int i = 0; i < 6; i++) randNum[i] = (double)rand() / RAND_MAX;//获取随机数
            double dQdtC = randNum[syncC];//决定使用3个随机数中的哪个
            double dQdtM = randNum[syncM];
            double dQdtA = randNum[syncA];
            if(distributionC){//非0则不使用均匀分布
                dQdtC = (dQdtC + randNum[syncC + 3]) / 2;//改为三角分布
            }
            if(distributionM){
                dQdtM = (dQdtM + randNum[syncM + 3]) / 2;
            }
            if(distributionA){
                dQdtA = (dQdtA + randNum[syncA + 3]) / 2;
            }
            if(positiveOnlyC >= 2) dQdtC -= 1.0;//改为负分布
            else if(!positiveOnlyC) dQdtC -= 0.5;//改为正负对称分布
            if(positiveOnlyM >= 2) dQdtM -= 1.0;
            else if(!positiveOnlyM) dQdtM -= 0.5;
            if(positiveOnlyA >= 2) dQdtA -= 1.0;
            else if(!positiveOnlyA) dQdtA -= 0.5;
            
            qdtC = qdtC.addSecs(dTimeC * dQdtC);//按范围加入随机波动
            qdtM = qdtM.addSecs(dTimeM * dQdtM);
            qdtA = qdtA.addSecs(dTimeA * dQdtA);
            resp = qSetFileTime(files[i], ifC ? (&qdtC) : NULL, 
                                          ifM ? (&qdtM) : NULL, 
                                          ifA ? (&qdtA) : NULL);//设置文件时间
        }
        
        if(resp != 0){//若失败
            QFileInfo fileinfo = QFileInfo(files[i]);//文件信息
            QString thisFileName = fileinfo.fileName();//文件名
            if(resp == 1) QMessageBox::critical(this, "提示", "无路径");
            else if(resp == 2) QMessageBox::critical(this, "提示", thisFileName + " 获取原时间失败");
            else if(resp == 3) QMessageBox::critical(this, "提示", thisFileName + " 设置时间失败");
        }
    }
}

void Widget::on_Bt_set_clicked()//修改时间
{
    UiSetFileTime(fileNow);//设置文件时间
    UiGetFileTime(fileNow);//重新获取文件时间
}

void Widget::on_Bt_set_Creat_clicked()//修改创建时间
{
    UiSetFileTime(fileNow, 0x01);//设置文件时间
    UiGetFileTime(fileNow);//重新获取文件时间
}
void Widget::on_Bt_set_Mod_clicked()//修改修改时间
{
    UiSetFileTime(fileNow, 0x02);//设置文件时间
    UiGetFileTime(fileNow);//重新获取文件时间
}
void Widget::on_Bt_set_Access_clicked()//修改访问时间
{
    UiSetFileTime(fileNow, 0x04);//设置文件时间
    UiGetFileTime(fileNow);//重新获取文件时间
}

void Widget::on_Bt_copy_Creat_clicked()//复制创建时间
{
    ui->dateTime_setCreat->setDateTime(ui->dateTime_getCreat->dateTime());
}
void Widget::on_Bt_copy_Mod_clicked()//复制修改时间
{
    ui->dateTime_setMod->setDateTime(ui->dateTime_getMod->dateTime());
}
void Widget::on_Bt_copy_Access_clicked()//复制访问时间
{
    ui->dateTime_setAccess->setDateTime(ui->dateTime_getAccess->dateTime());
}

void Widget::on_Bt_set_multiFileInc_clicked()//多文件时间递进
{
    multiFileHandle(0);
}

void Widget::on_Bt_set_multiFileRand_clicked()//多文件随机波动
{
    multiFileHandle(1);
}

void Widget::on_Bt_set_instruction_clicked()//说明
{
    QMessageBox::information(this, "操作说明", instructionStr);
}

void Widget::on_Bt_set_instructionM_clicked()//多文件说明
{
    QMessageBox::information(this, "多文件操作说明", instructionStrM);
}
