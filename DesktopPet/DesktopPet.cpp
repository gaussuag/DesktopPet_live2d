#include "DesktopPet.h"

#include <QMenu>
#include <QDesktopWidget>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QVBoxLayout>
#include <QTextCodec>
#include <QSlider>

#include <QSystemTrayIcon>
#include <QComboBox>
#include <QStandardItemModel>
#include <QToolButton>
#include <QFileDialog>
#include <QProcess>
#include <QAction>

#include <QLabel>

#include "Live2dWebView/Live2dWebView.h"
#include "CustomWidget/SettingsWidget.h"
#include "CustomWidget/CustomBubbleWidget.h"

DesktopPet::DesktopPet(QWidget *parent)
    : QWidget(parent)
    ,watchedFlag(false),adjustFlag(false),supCatFlag(false),wakeupflag(false)
{
    const char *codecname = "GBK";
    QTextCodec *codec = QTextCodec::codecForName(codecname);
    QTextCodec::setCodecForLocale(codec);

    initWidget();
    initSysTrayIcon();
    initSettingWidget();
    initAdjustWidget();
    initSupCatWidget();
    initBubbleWidget();
    initConnect();

    setWatchedCursor(true);

    initConfig();
    initSysAppCode();

    ModelView->setPosition("right");
    ModelView->setHorizonOffsetValue(currentmodelhoffset);
    ModelView->setVerticalOffsetValue(currentmodelvoffset);
    ModelView->setModelScaleValue(currentmodescale);
    ModelView->setModel(currentmodelName);

}

DesktopPet::~DesktopPet()
{
    setWatchedCursor(false);
}


bool DesktopPet::eventFilter(QObject *w,QEvent *e)
{
    if(adjustFlag)
    {
        if(e->type() == QEvent::KeyPress && w == adjustWidget)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);

            int position = ModelView->getPosition();
            if(keyEvent->key() == Qt::Key_Left)
            {
                if(position == 1)
                    ModelView->setHorizonOffset(QString("%1").arg(ModelView->getHorizonOffset() - 50));
                else if(position == 2)
                    ModelView->setHorizonOffset(QString("%1").arg(ModelView->getHorizonOffset() + 50));

            }
            else  if(keyEvent->key() == Qt::Key_Right)
            {
                if(position == 1)
                    ModelView->setHorizonOffset(QString("%1").arg(ModelView->getHorizonOffset() + 50));
                else if(position == 2)
                    ModelView->setHorizonOffset(QString("%1").arg(ModelView->getHorizonOffset() - 50));
            }
            else  if(keyEvent->key() == Qt::Key_Up)
                ModelView->setVerticalOffset(QString("%1").arg(ModelView->getVerticalOffset() + 50));
            else  if(keyEvent->key() == Qt::Key_Down)
                ModelView->setVerticalOffset(QString("%1").arg(ModelView->getVerticalOffset() - 50));
        }else if(e->type() == QEvent::MouseButtonPress && w == adjustWidget)
        {
            adjustAction->setChecked(false);
            emit adjustAction->triggered();
        }
    }
    return QWidget::eventFilter(w,e);
}


void DesktopPet::setWatchedCursor(bool flag)
{
    watchedFlag = flag;
    if(watchedFlag)
        ThreadRes = QtConcurrent::run(this,&DesktopPet::watchedCursor);
    else
    {
        if(ThreadRes.isRunning())
            ThreadRes.setPaused(true);
    }
}

void DesktopPet::initWidget()
{   
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::SubWindow);
    this->setAttribute(Qt::WA_TranslucentBackground);

    this->installEventFilter(this);

    setWindowIcon(QIcon(":/res/icon/baishikeleqishui32.png"));

    VBoxLayout = new QVBoxLayout(this);
    VBoxLayout->setSpacing(0);
    VBoxLayout->setMargin(0);

    initModelViewWidget();

    VBoxLayout->addWidget(ModelView);

    QDesktopWidget *deskW = QApplication::desktop();
    QRect rect = deskW->availableGeometry(this);

    this->setGeometry(0,0,rect.width(),rect.height());
}
void DesktopPet::initModelViewWidget()
{
    ModelView = new Live2dWebView(this);
}

void DesktopPet::initSysTrayIcon()
{   
    SysTrayIcon = new QSystemTrayIcon(QIcon(":/res/icon/baishikeleqishui32.png"), this);
    SysTrayIcon->setToolTip(QString::fromLocal8Bit("建议光速喝肥宅水"));
    SysTrayIcon->show();

    /* 创建托盘菜单 */
    QMenu* menu = new QMenu(this);
    menu->setStyleSheet("QMenu{"
                        "background-color:white;"
                        "border:1px solid white;"
                        "}"
                        "QMenu::item{"
                        "background-color:transparent;"
                        "padding:8px 32px;"
                        "margin:0px 8px;"
                        "}"
                        "QMenu::item:selected{"
                        "background-color:#2dabf9"
                        "}");
    QAction *setAction = new QAction(QIcon(":/res/icon/settings32.png"),QString("Settings"),this);
    connect(setAction,&QAction::triggered,this,&DesktopPet::setAction_slot);

    adjustAction = new QAction(QIcon(":/res/icon/weizhi.png"),QString::fromLocal8Bit("设置位置"),this);
    adjustAction->setCheckable(true);
    adjustAction->setChecked(false);
    connect(adjustAction,&QAction::triggered,this,&DesktopPet::adjustAction_slot);


    QAction *SupCatAction = new QAction(QIcon(":/res/icon/zhaocaimao.png"),QString::fromLocal8Bit("云吸猫大法"),this);
    connect(SupCatAction,&QAction::triggered,this,&DesktopPet::SupCatAction_slot);


    QAction *SnapAction = new QAction(QIcon(":/res/icon/jietu.png"),QString::fromLocal8Bit("保存这群小东西"),this);
    connect(SnapAction,&QAction::triggered,this,&DesktopPet::SnapAction_slot);

    QAction *quitAction = new QAction(QIcon(":/res/icon/exit32.png"),QString("Quit"),this);
    connect(quitAction,&QAction::triggered,qApp,&QApplication::quit);

    menu->addAction(adjustAction);
    menu->addAction(SupCatAction);
    menu->addAction(SnapAction);
    menu->addAction(setAction);
    menu->addAction(quitAction);
    SysTrayIcon->setContextMenu(menu);
}

void DesktopPet::initSettingWidget()
{
    settingsWidget = new SettingsWidget(this);
    settingsWidget->hide();

    connect(settingsWidget,SIGNAL(modelChange(QString)),ModelView,SLOT(setModel(QString)));
    connect(ModelView,SIGNAL(currentModelchange(QString)),settingsWidget,SLOT(currentModelchange_slot(QString)));

    connect(ModelView,SIGNAL(modelHoffsetChange(QString)),settingsWidget,SLOT(modelHoffsetChange_slot(QString)));
    connect(ModelView,SIGNAL(modelVoffsetChange(QString)),settingsWidget,SLOT(modelVoffsetChange_slot(QString)));
    connect(ModelView,SIGNAL(modelScaleChange(double)),settingsWidget,SLOT(modelScaleChange_slot(double)));

    connect(settingsWidget,SIGNAL(modelTestPath(QString)),ModelView,SLOT(modelTestByModelPath(QString)));

    settingsWidget->setModelMap(ModelView->getModelMap());
}

void DesktopPet::initAdjustWidget()
{
    adjustWidget = new QWidget(this);

    adjustWidget->installEventFilter(this);
    adjustWidget->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    adjustWidget->setWindowOpacity(0.5);


    QVBoxLayout *VBoxLayout = new QVBoxLayout();
    horScroll = new QSlider(Qt::Horizontal,adjustWidget);
    horScroll->setRange(0,this->rect().width());
    horScroll->setValue(ModelView->getHorizonOffset());
    connect(horScroll,&QSlider::valueChanged,this,[=](int value){
        ModelView->setHorizonOffset(QString("%1").arg(value));
    });

     //Set Model Scale
    QWidget *ScaleWidget = new QWidget(adjustWidget);
    ScaleWidget->setStyleSheet("background-color:blue;");
    ScaleWidget->setFixedSize(400,200);
    QVBoxLayout *ScaleLayout = new QVBoxLayout(ScaleWidget);

    QLabel *ScaleLabel = new QLabel(QString::fromLocal8Bit("设置模型大小"),ScaleWidget);
    ScaleLabel->setStyleSheet("font:18pt;color:red;");
    ScaleSlider = new QSlider(Qt::Orientation::Horizontal,ScaleWidget);
    ScaleSlider->setRange(5,22);
    connect(ScaleSlider,&QSlider::sliderMoved,this,[=](int value){
        double scale = value / 10.0;
        ModelView->setModelScale(scale);
    });

    ScaleLayout->addWidget(ScaleLabel);
    ScaleLayout->addWidget(ScaleSlider);

    VBoxLayout->addStretch();
    VBoxLayout->addWidget(ScaleWidget);
    VBoxLayout->addStretch();
    VBoxLayout->addWidget(horScroll);


    QHBoxLayout *HBoxLayout = new QHBoxLayout(adjustWidget);

    vorScroll = new QSlider(Qt::Vertical,adjustWidget);
    vorScroll->setRange(0,this->rect().height());
    vorScroll->setValue(ModelView->getVerticalOffset());
    connect(vorScroll,&QSlider::valueChanged,this,[=](int value){
        ModelView->setVerticalOffset(QString("%1").arg(value));
    });

    HBoxLayout->addLayout(VBoxLayout);
    HBoxLayout->addWidget(vorScroll);


    adjustWidget->hide();
}

void DesktopPet::initSupCatWidget()
{
    supCatWidget = new QWidget(this);
    supCatWidget->setAttribute(Qt::WA_StyledBackground);
    supCatWidget->setObjectName("supCatWidget");
    supCatWidget->setWindowFlags(Qt::Dialog);
    supCatWidget->setFixedSize(300,200);
    supCatWidget->setStyleSheet("QWidget#supCatWidget{background-color: white;"
                                "border:1px solid rgb(221,221,221);}");


    QVBoxLayout *VBoxLayout = new QVBoxLayout(supCatWidget);

    ChoiceCatNumBox = new QComboBox(supCatWidget);
    ChoiceCatNumBox->setStyleSheet("QComboBox{background-color: rgb(255, 255, 255);"
                                   "border:1px solid rgb(221,221,221);"
                                   "font: 14pt;"
                                   "padding-left:10px;}"
                                   "QComboBox::hover{background-color: rgb(200, 255, 255);}"
                               "QComboBox::down-arrow{image: url(:/res/icon/SupCatDownArrow.png);}"
                               "QComboBox::drop-down{"
                                   "width:20px;"
                                   "border:none;"
                                   "padding-right:10px;}");

    ChoiceCatNumBox->setMinimumHeight(40);
    ChoiceCatNumBox->setMaxCount(10);
    QStringList stringList;
    for(int i=0;i<10;i++)
        stringList.append(QString::fromLocal8Bit("就 %1 只猫吗？可能不够哦").arg(i+1));
    ChoiceCatNumBox->addItems(stringList);

    SortCatNumBox = new QComboBox(supCatWidget);
    SortCatNumBox->setStyleSheet("QComboBox{background-color: rgb(255, 255, 255);"
                                   "border:1px solid rgb(221,221,221);"
                                   "font: 14pt;"
                                   "padding-left:10px;}"
                                   "QComboBox::hover{background-color: rgb(200, 255, 255);}"
                               "QComboBox::down-arrow{image: url(:/res/icon/SupCatSort.png);}"
                               "QComboBox::drop-down{"
                                   "width:20px;"
                                   "border:none;"
                                   "padding-right:10px;}");

    SortCatNumBox->setMinimumHeight(40);
    SortCatNumBox->setMaxCount(2);
    QStringList SortstringList;
    SortstringList.append(QString::fromLocal8Bit("猫猫全部排排坐~"));
    SortstringList.append(QString::fromLocal8Bit("猫猫魔力转圈圈~"));
    SortCatNumBox->addItems(SortstringList);
    SortCatNumBox->setCurrentIndex(0);



    ShowCatBt = new QToolButton(supCatWidget);
    ShowCatBt->setIcon(QIcon(":/res/icon/SupCatShow.png"));
    ShowCatBt->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ShowCatBt->setStyleSheet("QToolButton{background-color: rgb(255, 255, 255);"
                             "border:1px solid rgb(221,221,221);"
                             "font: 10pt;}"
                             "QToolButton::hover{background-color: rgb(200, 255, 255);}");
    ShowCatBt->setText(QString::fromLocal8Bit("出来吧！猫咪们！"));
    ShowCatBt->setFixedSize(140,40);

    connect(ShowCatBt,&QToolButton::pressed,this,&DesktopPet::ShowCatBt_slot);

    HideCatBt = new QToolButton(supCatWidget);
    HideCatBt->setIcon(QIcon(":/res/icon/SupCatHome.png"));
    HideCatBt->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    HideCatBt->setStyleSheet("QToolButton{background-color: rgb(255, 255, 255);"
                             "border:1px solid rgb(221,221,221);"
                             "font: 10pt;}"
                             "QToolButton::hover{background-color: rgb(200, 255, 255);}");

    HideCatBt->setText(QString::fromLocal8Bit("还没有喵喵出来玩哦~"));
    HideCatBt->setFixedSize(140,40);
    connect(HideCatBt,&QToolButton::pressed,this,&DesktopPet::HideCatBt_slot);


    QHBoxLayout *HBoxLayout = new QHBoxLayout();
    HBoxLayout->addWidget(ShowCatBt);
    HBoxLayout->addWidget(HideCatBt);

    VBoxLayout->addWidget(ChoiceCatNumBox);
    VBoxLayout->addWidget(SortCatNumBox);

    VBoxLayout->addLayout(HBoxLayout);


    EqulSortList.append(QPair<int,int>(0,0));
    EqulSortList.append(QPair<int,int>(750,0));
    EqulSortList.append(QPair<int,int>(300,300));
    EqulSortList.append(QPair<int,int>(300,-300));
    EqulSortList.append(QPair<int,int>(450,300));
    EqulSortList.append(QPair<int,int>(450,-300));
    EqulSortList.append(QPair<int,int>(150,150));
    EqulSortList.append(QPair<int,int>(150,-150));
    EqulSortList.append(QPair<int,int>(600,150));
    EqulSortList.append(QPair<int,int>(600,-150));
}

void DesktopPet::initBubbleWidget()
{
    BubbleWidget = new CustomBubbleWidget(this);
    BubbleWidget->setBackgroundColor(QColor(180, 255, 255,180));
    BubbleLabel = new QLabel;
    BubbleLabel->setStyleSheet("font: 14pt \"SimHei\";");
//    BubbleLabel->setMinimumSize(200,200);
    BubbleLabel->setText(QString::fromLocal8Bit("我是小可爱~"));
    BubbleWidget->setCenterWidget(BubbleLabel);
    BubbleWidget->hide();
}

void DesktopPet::initConnect()
{
    connect(this,SIGNAL(CursorPointChange(QPoint)),this,SLOT(CursorPointChange_slot(QPoint)),Qt::QueuedConnection);

    //Setting set Config
    connect(settingsWidget,SIGNAL(wakeupwordChange(QString)),this,SLOT(wakeupwordChange_slot(QString)));
    connect(settingsWidget,SIGNAL(appNamePathMapChange(QMap<QString,QString>)),this,SLOT(appNamePathMapChange_slot(QMap<QString,QString>)));
}

void DesktopPet::initConfig()
{
    settingsWidget->getConfigParameter(autoRun,voiceEnable,wakeupword,appNamePathMap,currentmodelName,currentmodelhoffset,currentmodelvoffset,currentmodescale);

    horScroll->setValue(currentmodelhoffset.toInt());
    vorScroll->setValue(currentmodelvoffset.toInt());
    ScaleSlider->setValue(int(currentmodescale*10));

    if(voiceEnable == 2)
        emit(settingsWidget->voiceEnableStateChange(2));
}
void DesktopPet::initSysAppCode()
{
    SysAppMap.insert(QString::fromLocal8Bit("计算器"),QString("calc"));
    SysAppMap.insert(QString::fromLocal8Bit("记事本"),QString("notepad"));
    SysAppMap.insert(QString::fromLocal8Bit("画图"),QString("mspaint"));
    SysAppMap.insert(QString::fromLocal8Bit("任务管理器"),QString("taskmgr"));
    SysAppMap.insert(QString::fromLocal8Bit("截图"),QString("SnippingTool"));
}
void DesktopPet::watchedCursor()
{
    while (watchedFlag) {
        QPoint newPoint = this->cursor().pos();
        if(CurrentPoint != newPoint)
        {
            CurrentPoint = newPoint;
            emit(CursorPointChange(newPoint));
        }else
            QThread::msleep(50);
    }
}
void DesktopPet::CursorPointChange_slot(QPoint cursorPoint)
{
    ModelView->sendMouseByCursorPoint(cursorPoint);
    if(supCatFlag)
    {
        for(const auto & CatView : supCatList)
            CatView->sendMouseByCursorPoint(cursorPoint);
    }
}


void DesktopPet::setAction_slot()
{
    if(!settingsWidget->isVisible())
    {
        QPoint point = this->geometry().center() - settingsWidget->rect().center();
        settingsWidget->move(point);
        settingsWidget->show();
    }else
        settingsWidget->raise();
}
void DesktopPet::adjustAction_slot()
{
    adjustFlag = adjustAction->isChecked();
    if(adjustFlag)
    {
        adjustWidget->setGeometry(this->geometry());
        adjustWidget->show();
    }
    else
        adjustWidget->hide();
    qDebug()<<adjustFlag;
}

void DesktopPet::SupCatAction_slot()
{
    if(!supCatWidget->isVisible())
    {
        QPoint point = this->geometry().center() - supCatWidget->rect().center();
        supCatWidget->move(point);
        supCatWidget->show();
    }else
        supCatWidget->raise();

}
void DesktopPet::ShowCatBt_slot()
{
    int baseoffset = 500;
    if(!supCatFlag)
    {
        supCatFlag = true;
        int CatCount = ChoiceCatNumBox->currentIndex() + 1;

        // 0 排排坐 1 围圈圈
        int ShowSortType = SortCatNumBox->currentIndex();
        QString CatName;
        QRect rect = this->geometry();
        for(int i=0;i<CatCount;i++)
        {

            if((rand() % 2) == 0)
                CatName = "hijiki";
            else
                CatName = "tororo";

            Live2dWebView *Cat = new Live2dWebView(this);
            Cat->setGeometry(rect);
            if(ShowSortType == 0)
            {
                Cat->setCatHorizonOffsetValue(QString("%1").arg(i * 100 + baseoffset));
                Cat->setCatVerticalOffsetValue(QString("%1").arg(baseoffset));
            }else if(ShowSortType == 1)
            {
                Cat->setCatHorizonOffsetValue(QString("%1").arg(EqulSortList.at(i).first + baseoffset));
                Cat->setCatVerticalOffsetValue(QString("%1").arg(EqulSortList.at(i).second +baseoffset));
            }
            Cat->setCatModel(CatName);

            supCatList.append(Cat);
            Cat->show();
        }

        ShowCatBt->setCheckable(false);
        ShowCatBt->setText(QString::fromLocal8Bit(" 喵喵  喵喵喵~"));

        HideCatBt->setText(QString::fromLocal8Bit("让猫咪们回家吧~"));
    }

}

void DesktopPet::SnapAction_slot()
{
    QPixmap pixmap(this->size());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    this->render(&painter);

    QString savePicPath = QFileDialog::getSaveFileName(this,QString::fromLocal8Bit("保存这群小东西"));

    if(!savePicPath.contains(".png"))
        savePicPath = savePicPath + ".png";

    pixmap.save(savePicPath,"PNG",100);
}

void DesktopPet::HideCatBt_slot()
{
    if(supCatFlag)
    {
        supCatFlag = false;
        for(auto & CatView : supCatList)
        {
            CatView->hide();
            delete  CatView;
            CatView = nullptr;
        }
        supCatList.clear();

        ShowCatBt->setCheckable(true);
        ShowCatBt->setText(QString::fromLocal8Bit("出来吧！猫咪们！"));

        HideCatBt->setText(QString::fromLocal8Bit("还没有喵喵出来玩哦~"));
    }
}


//SettingWidget set Config
void DesktopPet::wakeupwordChange_slot(QString word)
{
    wakeupword = word;
}

void DesktopPet::appNamePathMapChange_slot(QMap<QString,QString> AppNamePathMap)
{
    appNamePathMap.clear();
    appNamePathMap = AppNamePathMap;
}

//Model Control
void DesktopPet::checkWakeupOption(const QString &string)
{
    if(QString::compare(string,wakeupword) ==0)
    {
        wakeupflag = true;

        if(!BubbleWidget->isVisible())
        {
            BubbleLabel->setText(QString::fromLocal8Bit("我是%1,你说呗~").arg(ModelView->getModelName()));
            QRect rect = ModelView->getModelRectWidget();
            BubbleWidget->move(rect.center().x() + rect.width()/2,rect.center().y());
            BubbleWidget->show();
        }
    }
}
void DesktopPet::handleModelCode(const QString &string)
{
    if(checkOutOption(string))
        return;
    if(checkCodeAboutRunApp(string))
        return;
    if(checkModelAction(string))
        return;
}
bool DesktopPet::checkOutOption(const QString &string)
{
    static QStringList sleepCode;
    if(sleepCode.size() == 0)
    {
        sleepCode.append(QString::fromLocal8Bit("拜拜"));
        sleepCode.append(QString::fromLocal8Bit("再见"));
        sleepCode.append(QString::fromLocal8Bit("Bye bye"));
    }
    //check Out code
    for(const auto & code : sleepCode)
    {
        if(string.contains(code))
        {
            wakeupflag = false;

            if(BubbleWidget->isVisible())
                QtConcurrent::run(this,&DesktopPet::hideBubbleWidget);


            return true;
        }
    }
    return false;
}
bool DesktopPet::checkCodeAboutRunApp(const QString &string)
{
    const QList<QString> &SysAppNameList = SysAppMap.keys();
    for(const auto & SysAppName : SysAppNameList)
    {
        if(string.contains(SysAppName))
        {
            runSysAppByApplicationCode(SysAppName);
            return true;
        }
    }

    const QList<QString> &AppNameList = appNamePathMap.keys();
    for(const auto & AppName : AppNameList)
    {
        if(string.contains(AppName))
        {
            runAppByApplicationName(AppName);
            return true;
        }
    }
    return false;
}
void DesktopPet::runSysAppByApplicationCode(const QString &AppName)
{
    if(SysAppMap.contains(AppName))
    {

        QString AppCode = SysAppMap.value(AppName);
//        system(AppCode.toLocal8Bit());
        QProcess p(0);
        p.start("cmd");
        p.waitForStarted();

        p.write(AppCode.toLocal8Bit() + "\n");

        p.closeWriteChannel();

        p.waitForFinished();
    }
}
void DesktopPet::runAppByApplicationName(const QString &AppName)
{
    if(appNamePathMap.contains(AppName))
    {

        QString AppPath = appNamePathMap.value(AppName);
        QFileInfo fileInfo(AppPath);
        if(fileInfo.isExecutable())
            QProcess::startDetached(AppPath);
    }

}

bool DesktopPet::checkModelAction(const QString  &string)
{
    if(QString::compare(string,QString::fromLocal8Bit("向左看")) == 0 ||
            QString::compare(string,QString::fromLocal8Bit("看左边")) == 0)
    {
        qDebug()<<"left";
        CursorPointChange_slot(QPoint(200,600));
        return true;
    }else if(QString::compare(string,QString::fromLocal8Bit("向右看")) == 0 ||
             QString::compare(string,QString::fromLocal8Bit("看右边")) == 0)
    {
        qDebug()<<"right";
        CursorPointChange_slot(QPoint(1700,600));
        return true;
    }else if(QString::compare(string,QString::fromLocal8Bit("向上看")) == 0 ||
             QString::compare(string,QString::fromLocal8Bit("看上边")) == 0 ||
             QString::compare(string,QString::fromLocal8Bit("看上面")) == 0)
    {
        qDebug()<<"up";
        CursorPointChange_slot(QPoint(900,200));
        return true;
    }else if(QString::compare(string,QString::fromLocal8Bit("向下看")) == 0 ||
             QString::compare(string,QString::fromLocal8Bit("看下边")) == 0 ||
             QString::compare(string,QString::fromLocal8Bit("看下面")) == 0)
    {
        qDebug()<<"down";
        CursorPointChange_slot(QPoint(900,1000));
        return true;
    }
    else if(QString::compare(string,QString::fromLocal8Bit("转圈圈")) == 0 ||
                 QString::compare(string,QString::fromLocal8Bit("转一圈")) == 0)
    {
        QtConcurrent::run(this,&DesktopPet::modelLookAround);
        return true;
    }

    return false;
}

void DesktopPet::modelLookAround()
{
    ModelView->sendMouseByCursorPoint(QPoint(330,400));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(480,280));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(600,225));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(820,210));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(1094,242));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(1240,300));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(1250,430));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(1180,560));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(1080,680));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(970,785));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(750,800));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(540,760));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(400,710));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(350,625));
    QThread::msleep(188);

    ModelView->sendMouseByCursorPoint(QPoint(225,595));
    QThread::msleep(188);
}

void DesktopPet::hideBubbleWidget()
{
    BubbleLabel->setText(QString::fromLocal8Bit("好咯~拜拜~"));
    QThread::sleep(3);
    BubbleWidget->hide();
}
