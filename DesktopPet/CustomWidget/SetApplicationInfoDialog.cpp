#include "SetApplicationInfoDialog.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QTextCodec>
#include <QMessageBox>

#include "CustomToolButton.h"

SetApplicationInfoDialog::SetApplicationInfoDialog(QString AppPath,QWidget *parent) :
    QDialog(parent),appPath(AppPath)
{
    const char *codecname = "GBK";
    QTextCodec *codec = QTextCodec::codecForName(codecname);
    QTextCodec::setCodecForLocale(codec);

    initWidget();
    setFocus();
    wakeupApplicationLineEdit->setFocus();

}

SetApplicationInfoDialog::~SetApplicationInfoDialog()
{

}

void SetApplicationInfoDialog::initWidget()
{
    setObjectName("SetApplicationInfoDialog");
    setStyleSheet("QDialog#SetApplicationInfoDialog{background-color:white;}");
    QVBoxLayout *VBoxLayout = new QVBoxLayout(this);

    QLabel *wakeupApplicationName = new QLabel(QString::fromLocal8Bit("设置这个程序的启动词"),this);
    wakeupApplicationName->setStyleSheet("font:12pt;");
    wakeupApplicationName->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    wakeupApplicationLineEdit = new QLineEdit(this);
    wakeupApplicationLineEdit->setMinimumHeight(40);
    wakeupApplicationLineEdit->setStyleSheet("font:12pt;");
    wakeupApplicationLineEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    CustomToolButton *ComfirmBt = new CustomToolButton(this);
    ComfirmBt->setText("OK");
    ComfirmBt->setFixedSize(60,30);
    ComfirmBt->setStyleSheet("CustomToolButton{background-color: rgb(221, 255, 255);"
                           "border:1px solid rgb(221,221,221);"
                           "border-radius:10px;"
                           "font: 10pt;}"
                           "CustomToolButton::hover{background-color: rgb(180, 255, 255);}");
    connect(ComfirmBt,&CustomToolButton::pressed,this,&SetApplicationInfoDialog::ComfirmBt_pressed_slot);


    QHBoxLayout *HboxLayout = new QHBoxLayout();
    HboxLayout->addStretch();
    HboxLayout->addWidget(ComfirmBt);

    VBoxLayout->addWidget(wakeupApplicationName);
    VBoxLayout->addWidget(wakeupApplicationLineEdit);
    VBoxLayout->addLayout(HboxLayout);

}

void SetApplicationInfoDialog::ComfirmBt_pressed_slot()
{
    QString AppName = QString::fromLocal8Bit(wakeupApplicationLineEdit->text().toLocal8Bit());
    if(AppName.isEmpty())
    {
        QMessageBox::information(this,QString::fromLocal8Bit("设置软件名字"),QString::fromLocal8Bit("要设置软件的启动词哦！"));
        return;
    }

    QPair<QString,QString> AppInfo;
    AppInfo.first = AppName;
    AppInfo.second = appPath;

   emit(AddApplicationInfo(AppInfo));

    this->close();
}
