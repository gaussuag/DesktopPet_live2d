#include "SettingsWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QIcon>
#include <QAction>

#include <QCheckBox>

#include <QApplication>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <QFileInfo>
#include <QDialog>

#include <QDebug>

#include "ModelSetWidget.h"
#include "CustomToolButton.h"
#include "SetApplicationInfoDialog.h"



SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent)
{
    initWidget();
    initConfig();
    initSettingByConfig();
}

SettingsWidget::~SettingsWidget()
{

}

void SettingsWidget::getConfigParameter(int &AutoRun, int &VoiceEnable, QString &WakeupWord
                                        , QMap<QString, QString> &AppNamePathMap
                                        , QString &ModelName, QString &Hoffset, QString &Voffset, double &ScaleValue)
{
    AutoRun = autoRun;
    VoiceEnable = voiceEnable;

    WakeupWord = wakeupword;

    AppNamePathMap = appNamePathMap;

    ModelName = modelName;
    Hoffset = hoffset;
    Voffset = voffset;
    ScaleValue = scaleValue;
}


void SettingsWidget::setModelMap(QMap<QString,QJsonObject> map)
{
    modelWidget->setModelMap(map);
}



void SettingsWidget::initWidget()
{
    setWindowFlags(Qt::Dialog);
    setObjectName("SettingsWidget");
    setStyleSheet("QWidget#SettingsWidget{background-color:white;"
                  "border-top:1px solid gray;"
                  "border-bottom:0px solid gray;}");

    QVBoxLayout *VBoxLayout = new QVBoxLayout(this);
    VBoxLayout->setSpacing(5);
    VBoxLayout->setContentsMargins(5,0,5,5);

    initModelSetWidget();
    initOptionSetWidget();
    initModelTestWidget();

    VBoxLayout->addWidget(modelWidget);
    VBoxLayout->addWidget(optionSetWidget);
    VBoxLayout->addWidget(ModelTestWidget);
    VBoxLayout->addStretch();

    optionSetWidget->hide();
}
void SettingsWidget::initModelSetWidget()
{
    modelWidget = new ModelSetWidget(this);
    modelWidget->setFixedHeight(270);
    modelWidget->setObjectName("ModelSetWidget");
    modelWidget->setStyleSheet("ModelSetWidget{background-color:white;"
                               "border:2px solid gray;}");
    connect(modelWidget,&ModelSetWidget::modelChange,this,[=](QString string){
        emit(modelChange(string));
    });
}

void SettingsWidget::initOptionSetWidget()
{
    optionSetWidget = new QWidget(this);

    optionSetWidget->setMaximumHeight(350);

    optionSetWidget->setObjectName("optionSetWidget");
    optionSetWidget->setStyleSheet("QWidget#optionSetWidget{background-color:white;"
                                   "border:2px solid gray;}");

    QVBoxLayout *VBoxLayout = new QVBoxLayout(optionSetWidget);
    VBoxLayout->setSpacing(5);
    VBoxLayout->setMargin(5);

    autoRunCheckBox = new QCheckBox(optionSetWidget);
    autoRunCheckBox->setStyleSheet("font:12pt;");
    autoRunCheckBox->setMinimumHeight(40);
    autoRunCheckBox->setText(QString::fromLocal8Bit("开机自动运行"));
    connect(autoRunCheckBox,&QCheckBox::stateChanged,this,&SettingsWidget::autoRunCheckBox_stateChanged_slot);

    voiceEnableCheckBox = new QCheckBox(optionSetWidget);
    voiceEnableCheckBox->setStyleSheet("font:12pt;");
    voiceEnableCheckBox->setMinimumHeight(40);
    voiceEnableCheckBox->setText(QString::fromLocal8Bit("语音控制"));
    connect(voiceEnableCheckBox,&QCheckBox::stateChanged,this,&SettingsWidget::voiceEnableCheckBox_stateChanged_slot,Qt::QueuedConnection);

    QLabel *WakeupwordLabel = new QLabel(QString::fromLocal8Bit("语音唤醒词"),optionSetWidget);
    WakeupwordLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    wakeupWordLineEdit = new QLineEdit(optionSetWidget);
    wakeupWordLineEdit->setMinimumHeight(40);
    wakeupWordLineEdit->addAction(QIcon(":/res/icon/queding.png"),QLineEdit::TrailingPosition);
    wakeupWordLineEdit->setStyleSheet("font:12pt;");
    QAction * action = wakeupWordLineEdit->actions().at(0);
    connect(action,SIGNAL(triggered(bool)),this,SLOT(wakeupWordLineEdit_trigger_slot(bool)));

    //Appilcation Helper
    QWidget *AppRunHelpWidget = new QWidget(optionSetWidget);
    AppRunHelpWidget->setObjectName("AppRunHelpWidget");
    AppRunHelpWidget->setMinimumHeight(150);
    AppRunHelpWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
    AppRunHelpWidget->setStyleSheet("QWidget#AppRunHelpWidget{background-color:white;"
                                   "border:2px solid gray;}");
    QVBoxLayout *AppRunHelpLayout = new QVBoxLayout(AppRunHelpWidget);
    CustomToolButton *AddApplicationBt = new CustomToolButton(AppRunHelpWidget);
    AddApplicationBt->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    AddApplicationBt->setIconSize(QSize(350,135));
    AddApplicationBt->setIcon(QIcon(":/res/icon/addApplicationIcon.png"));
    AddApplicationBt->setTransparentStyleSheet();
    AddApplicationBt->setAcceptDrops(true);
    connect(AddApplicationBt,SIGNAL(dropUrlsChange(QList<QUrl>)),this,SLOT(AddApplicationBt_dropUrlsChange_slot(QList<QUrl>)));
    AppRunHelpLayout->addWidget(AddApplicationBt);




    VBoxLayout->addWidget(autoRunCheckBox);
    VBoxLayout->addWidget(voiceEnableCheckBox);
    VBoxLayout->addWidget(WakeupwordLabel);
    VBoxLayout->addWidget(wakeupWordLineEdit);
    VBoxLayout->addWidget(AppRunHelpWidget);
}

void SettingsWidget::initModelTestWidget()
{
    ModelTestWidget = new QWidget(this);
    ModelTestWidget->setObjectName("ModelTestWidget");
    ModelTestWidget->setStyleSheet("QWidget#ModelTestWidget{background-color:white;"
                                   "border:2px solid gray;}");

    QVBoxLayout *VBoxLayout = new QVBoxLayout(ModelTestWidget);

    QLabel *ModelTestLabel = new QLabel(QString::fromLocal8Bit("模型测试"),ModelTestWidget);
    ModelTestLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QLineEdit *lineEdit = new QLineEdit(ModelTestWidget);
    lineEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lineEdit->setMinimumHeight(50);

    QToolButton *ComfirmBt = new QToolButton(ModelTestWidget);
    ComfirmBt->setIcon(QIcon(":/res/icon/queding.png"));
    ComfirmBt->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ComfirmBt->setStyleSheet("QToolButton{background-color: rgb(221, 255, 255);"
                             "border:1px solid rgb(221,221,221);"
                             "border-radius:10px;"
                             "font: 10pt;}"
                             "QToolButton::hover{background-color: rgb(180, 255, 255);}");

    ComfirmBt->setText(QString::fromLocal8Bit("确定"));
    ComfirmBt->setFixedSize(80,40);

    connect(ComfirmBt,&QToolButton::pressed,this,[=](){
        emit(modelTestPath(lineEdit->text()));
    });

    VBoxLayout->addWidget(ModelTestLabel);
    VBoxLayout->addWidget(lineEdit);
    VBoxLayout->addWidget(ComfirmBt);
}

void SettingsWidget::initConfig()
{
    QFile configfile(QString("%1/config.json").arg(QApplication::applicationDirPath()));

    if(!configfile.exists())
    {
        if(generalConfig())
            return initConfig();
        else
            return;
    }

    configfile.open(QIODevice::ReadOnly|QIODevice::Text);
    QString configString = configfile.readAll();
    configfile.close();

    QJsonParseError pareseError;
    configJsonDocument = QJsonDocument::fromJson(configString.toUtf8(),&pareseError);

    if(!(pareseError.error == QJsonParseError::NoError))
        return;

    QJsonObject jsonObject = configJsonDocument.object();
    if(jsonObject.contains(QStringLiteral("config")))
    {
        //autorun
        QJsonObject configObj = jsonObject.value(QStringLiteral("config")).toObject();
        autoRun = configObj["autorun"].toInt();

        if(configObj.contains(QStringLiteral("voiceconfig")))
        {
            QJsonObject voiceConfigObj = configObj.value(QStringLiteral("voiceconfig")).toObject();

            wakeupword = voiceConfigObj["wakeupword"].toString();
            voiceEnable = voiceConfigObj["voiceEnable"].toInt();

            if(voiceConfigObj.contains(QStringLiteral("runapp")))
            {
                QJsonValue runappValue = voiceConfigObj.value("runapp");
                if(runappValue.isArray())
                {
                    QJsonArray runappArray = runappValue.toArray();
                    for(int i=0;i<runappArray.size();i++)
                    {
                        QJsonObject appArgObj = runappArray.at(i).toObject();
                        appNamePathMap.insert(appArgObj["appname"].toString(),appArgObj["apppath"].toString());
                    }
                }
            }
        }
        if(configObj.contains(QStringLiteral("currentModel")))
        {
            QJsonObject modelObj = configObj.value(QStringLiteral("currentModel")).toObject();
            modelName = modelObj["name"].toString();
            hoffset = modelObj["hoffset"].toString();
            voffset = modelObj["voffset"].toString();
            scaleValue = modelObj["scale"].toDouble();
        }

    }
}

void SettingsWidget::initSettingByConfig()
{
    Qt::CheckState autoRunstate = (autoRun == 0) ? Qt::CheckState::Unchecked : Qt::CheckState::Checked;
    autoRunCheckBox->setCheckState(autoRunstate);

    Qt::CheckState voiceEnablestate = (voiceEnable == 0) ? Qt::CheckState::Unchecked : Qt::CheckState::Checked;
    voiceEnableCheckBox->setCheckState(voiceEnablestate);

    wakeupWordLineEdit->setText(wakeupword);
}

bool SettingsWidget::generalConfig()
{
    QJsonObject mainObj;
    QJsonObject configObj;
    QJsonObject autorun;
    autorun.insert("autorun",QJsonValue("0"));

    QJsonObject currentModelObj;
    currentModelObj.insert("name",QJsonValue("tororo"));
    currentModelObj.insert("hoffset",QJsonValue(""));
    currentModelObj.insert("voffset",QJsonValue(""));
    currentModelObj.insert("scale",QJsonValue(""));


    configObj.insert("autorun",autorun);
    configObj.insert("currentModel",currentModelObj);

    mainObj.insert("config",configObj);

    QJsonDocument doc(mainObj);
    QByteArray data = doc.toJson();
    QFile configfile(QString("%1/config.json").arg(QApplication::applicationDirPath()));
    configfile.open(QIODevice::WriteOnly);
    configfile.write(data);
    configfile.close();

    return configfile.exists();
}

void SettingsWidget::AddApplicationBt_dropUrlsChange_slot(QList<QUrl> urls)
{
    for(const auto & url : urls)
    {
        QFileInfo file(url.toLocalFile());
        if(file.isExecutable())
            saveApplicationRequest(url.toLocalFile());
    }
}

void SettingsWidget::currentModelchange_slot(QString modelName)
{
    modelWidget->setCurrentModelPreViewLabel(modelName);

    modifiedConfigJsonModelNameValue(modelName);
}
void SettingsWidget::modelHoffsetChange_slot(QString Modelhoffset)
{
    modifiedConfigJsonModelhoffsetValue(Modelhoffset);
}
void SettingsWidget::modelVoffsetChange_slot(QString Modelvoffset)
{
    modifiedConfigJsonModelvoffsetValue(Modelvoffset);
}
void SettingsWidget::modelScaleChange_slot(double value)
{
    modifiedConfigJsonModelScaleValue(value);
}


void SettingsWidget::autoRunCheckBox_stateChanged_slot(int value)
{
    autoRun = value;

    modifiedConfigJsonAutoRunValue(autoRun);
}
void SettingsWidget::voiceEnableCheckBox_stateChanged_slot(int value)
{
    voiceEnable = value;

    modifiedConfigJsonVoiceEnableValue(voiceEnable);

    emit(voiceEnableStateChange(value));
}
void SettingsWidget::wakeupWordLineEdit_trigger_slot(bool)
{
    wakeupword = QString::fromLocal8Bit(wakeupWordLineEdit->text().toLocal8Bit());

    modifiedConfigJsonWakeupWord(wakeupword);

    emit(wakeupwordChange(wakeupword));
}


void SettingsWidget::modifiedConfigJsonWakeupWord(QString word)
{
    QJsonObject jsonObject = configJsonDocument.object();

    if(jsonObject.contains(QStringLiteral("config")))
    {
        QJsonObject configObj = jsonObject.take(QStringLiteral("config")).toObject();
        if(configObj.contains(QStringLiteral("voiceconfig")))
        {
            QJsonObject voiceConfigObj = configObj.take(QStringLiteral("voiceconfig")).toObject();
            QJsonValue wakeupwordObj = voiceConfigObj.take(QStringLiteral("wakeupword"));
            wakeupwordObj = word;

            voiceConfigObj.insert(QStringLiteral("wakeupword"),wakeupwordObj);
            configObj.insert(QStringLiteral("voiceconfig"),QJsonValue(voiceConfigObj));
        }
        jsonObject.insert(QStringLiteral("config"),configObj);

        configJsonDocument.setObject(jsonObject);

        modifiedConfigJsonFile(configJsonDocument.toJson());
    }
}
void SettingsWidget::modifiedConfigJsonAutoRunValue(int value)
{
    QJsonObject jsonObject = configJsonDocument.object();

    if(jsonObject.contains(QStringLiteral("config")))
    {
        QJsonObject configObj = jsonObject.take(QStringLiteral("config")).toObject();
        if(configObj.contains(QStringLiteral("autorun")))
        {
            QJsonValue autorunValue = configObj.take(QStringLiteral("autorun"));
            autorunValue = value;

            configObj.insert(QStringLiteral("autorun"),autorunValue);
        }
        jsonObject.insert(QStringLiteral("config"),configObj);

        configJsonDocument.setObject(jsonObject);

        modifiedConfigJsonFile(configJsonDocument.toJson());
    }
}
void SettingsWidget::modifiedConfigJsonVoiceEnableValue(int value)
{
    QJsonObject jsonObject = configJsonDocument.object();

    if(jsonObject.contains(QStringLiteral("config")))
    {
        QJsonObject configObj = jsonObject.take(QStringLiteral("config")).toObject();
        if(configObj.contains(QStringLiteral("voiceconfig")))
        {
            QJsonObject voiceConfigObj = configObj.take(QStringLiteral("voiceconfig")).toObject();
            QJsonValue voiceEnableValue = voiceConfigObj.take(QStringLiteral("voiceEnable"));
            voiceEnableValue = value;

            voiceConfigObj.insert(QStringLiteral("voiceEnable"),voiceEnableValue);
            configObj.insert(QStringLiteral("voiceconfig"),QJsonValue(voiceConfigObj));
        }
        jsonObject.insert(QStringLiteral("config"),configObj);

        configJsonDocument.setObject(jsonObject);

        modifiedConfigJsonFile(configJsonDocument.toJson());
    }
}
void SettingsWidget::modifiedConfigJsonModelNameValue(QString name)
{
    QJsonObject jsonObject = configJsonDocument.object();

    if(jsonObject.contains(QStringLiteral("config")))
    {
        QJsonObject configObj = jsonObject.take(QStringLiteral("config")).toObject();
        if(configObj.contains(QStringLiteral("currentModel")))
        {
            QJsonObject currentModelObj = configObj.take(QStringLiteral("currentModel")).toObject();
            QJsonValue nameValue = currentModelObj.take(QStringLiteral("name"));
            nameValue = name;

            currentModelObj.insert(QStringLiteral("name"),nameValue);
            configObj.insert(QStringLiteral("currentModel"),QJsonValue(currentModelObj));
        }
        jsonObject.insert(QStringLiteral("config"),configObj);

        configJsonDocument.setObject(jsonObject);

        modifiedConfigJsonFile(configJsonDocument.toJson());
    }
}
void SettingsWidget::modifiedConfigJsonModelhoffsetValue(QString Modelhoffset)
{
    QJsonObject jsonObject = configJsonDocument.object();

    if(jsonObject.contains(QStringLiteral("config")))
    {
        QJsonObject configObj = jsonObject.take(QStringLiteral("config")).toObject();
        if(configObj.contains(QStringLiteral("currentModel")))
        {
            QJsonObject currentModelObj = configObj.take(QStringLiteral("currentModel")).toObject();
            QJsonValue ModelhoffsetValue = currentModelObj.take(QStringLiteral("hoffset"));
            ModelhoffsetValue = Modelhoffset;

            currentModelObj.insert(QStringLiteral("hoffset"),ModelhoffsetValue);
            configObj.insert(QStringLiteral("currentModel"),QJsonValue(currentModelObj));
        }
        jsonObject.insert(QStringLiteral("config"),configObj);

        configJsonDocument.setObject(jsonObject);

        modifiedConfigJsonFile(configJsonDocument.toJson());
    }
}
void SettingsWidget::modifiedConfigJsonModelvoffsetValue(QString Modelvoffset)
{
    QJsonObject jsonObject = configJsonDocument.object();

    if(jsonObject.contains(QStringLiteral("config")))
    {
        QJsonObject configObj = jsonObject.take(QStringLiteral("config")).toObject();
        if(configObj.contains(QStringLiteral("currentModel")))
        {
            QJsonObject currentModelObj = configObj.take(QStringLiteral("currentModel")).toObject();
            QJsonValue ModelvoffsetValue = currentModelObj.take(QStringLiteral("voffset"));
            ModelvoffsetValue = Modelvoffset;

            currentModelObj.insert(QStringLiteral("voffset"),ModelvoffsetValue);
            configObj.insert(QStringLiteral("currentModel"),QJsonValue(currentModelObj));
        }
        jsonObject.insert(QStringLiteral("config"),configObj);

        configJsonDocument.setObject(jsonObject);

        modifiedConfigJsonFile(configJsonDocument.toJson());
    }
}
void SettingsWidget::modifiedConfigJsonModelScaleValue(double value)
{
    QJsonObject jsonObject = configJsonDocument.object();

    if(jsonObject.contains(QStringLiteral("config")))
    {
        QJsonObject configObj = jsonObject.take(QStringLiteral("config")).toObject();
        if(configObj.contains(QStringLiteral("currentModel")))
        {
            QJsonObject currentModelObj = configObj.take(QStringLiteral("currentModel")).toObject();
            QJsonValue ModelscaleValue = currentModelObj.take(QStringLiteral("scale"));
            ModelscaleValue = value;

            currentModelObj.insert(QStringLiteral("scale"),ModelscaleValue);
            configObj.insert(QStringLiteral("currentModel"),QJsonValue(currentModelObj));
        }
        jsonObject.insert(QStringLiteral("config"),configObj);

        configJsonDocument.setObject(jsonObject);

        modifiedConfigJsonFile(configJsonDocument.toJson());
    }
}
void SettingsWidget::modifiedConfigJsonFile(QByteArray json)
{
    QFile configfile(QString("%1/config.json").arg(QApplication::applicationDirPath()));

    if(!configfile.exists())
        return;

    configfile.open(QIODevice::WriteOnly|QIODevice::Text);
    configfile.seek(0);
    configfile.write(json);
    configfile.flush();

    configfile.close();
}

//add Application Request
void SettingsWidget::saveApplicationRequest(const QString &AppPath)
{
    SetApplicationInfoDialog *dialog = new SetApplicationInfoDialog(AppPath,this);
    connect(dialog,SIGNAL(AddApplicationInfo(QPair<QString,QString>)),this,SLOT(AddApplicationInfo_slot(QPair<QString,QString>)));
    dialog->exec();
    disconnect(dialog,SIGNAL(AddApplicationInfo(QPair<QString,QString>)),this,SLOT(AddApplicationInfo_slot(QPair<QString,QString>)));
}
void SettingsWidget::AddApplicationInfo_slot(QPair<QString,QString> AppInfo)
{
    AddConfigJSonRunApp(AppInfo);

    refreshappNamePathMap();
}
void SettingsWidget::AddConfigJSonRunApp(const QPair<QString,QString> &AppInfo)
{
    QJsonObject jsonObject = configJsonDocument.object();

    if(jsonObject.contains(QStringLiteral("config")))
    {
        QJsonObject configObj = jsonObject.take(QStringLiteral("config")).toObject();
        if(configObj.contains(QStringLiteral("voiceconfig")))
        {
            QJsonObject voiceConfigObj = configObj.take(QStringLiteral("voiceconfig")).toObject();
            QJsonValue runappValue = voiceConfigObj.take(QStringLiteral("runapp"));
            QJsonArray runappArray = runappValue.toArray();

            QJsonObject newRunAppObj;
            newRunAppObj.insert(QStringLiteral("appname"),QJsonValue(AppInfo.first));
            newRunAppObj.insert(QStringLiteral("apppath"),QJsonValue(AppInfo.second));

            runappArray.append(QJsonValue(newRunAppObj));

            voiceConfigObj.insert(QStringLiteral("runapp"),runappArray);
            configObj.insert(QStringLiteral("voiceconfig"),QJsonValue(voiceConfigObj));
        }
        jsonObject.insert(QStringLiteral("config"),configObj);

        configJsonDocument.setObject(jsonObject);

        modifiedConfigJsonFile(configJsonDocument.toJson());
    }
}

void SettingsWidget::refreshappNamePathMap()
{
    appNamePathMap.clear();

    QJsonObject jsonObject = configJsonDocument.object();
    if(jsonObject.contains(QStringLiteral("config")))
    {
        //autorun
        QJsonObject configObj = jsonObject.value(QStringLiteral("config")).toObject();

        if(configObj.contains(QStringLiteral("voiceconfig")))
        {
            QJsonObject voiceConfigObj = configObj.value(QStringLiteral("voiceconfig")).toObject();

            if(voiceConfigObj.contains(QStringLiteral("runapp")))
            {
                QJsonValue runappValue = voiceConfigObj.value("runapp");
                if(runappValue.isArray())
                {
                    QJsonArray runappArray = runappValue.toArray();
                    for(int i=0;i<runappArray.size();i++)
                    {
                        QJsonObject appArgObj = runappArray.at(i).toObject();
                        appNamePathMap.insert(appArgObj["appname"].toString(),appArgObj["apppath"].toString());
                    }
                }
            }
        }
    }

    emit(appNamePathMapChange(appNamePathMap));
}
