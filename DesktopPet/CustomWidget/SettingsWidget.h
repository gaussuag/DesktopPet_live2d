#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QMap>
#include <QJsonDocument>
#include <QUrl>
#include <QList>


class QCheckBox;
class QLineEdit;
class ModelSetWidget;


class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

    void getConfigParameter(int &AutoRun, int &VoiceEnable,QString & WakeupWord
                            , QMap<QString, QString> &AppNamePathMap
                            , QString &ModelName, QString &Hoffset, QString &Voffset,double &ScaleValue);
    void setModelMap(QMap<QString, QJsonObject> map);
private:
    ModelSetWidget *modelWidget;

    //OptionSet
    QWidget *optionSetWidget;
    QCheckBox *autoRunCheckBox;
    QCheckBox *voiceEnableCheckBox;
    QLineEdit *wakeupWordLineEdit;

    QWidget *ModelTestWidget;

    //Config
    QJsonDocument configJsonDocument;
    int autoRun;
    int voiceEnable;
    QString wakeupword;
    QMap<QString,QString> appNamePathMap;
    QString modelName;
    QString hoffset;
    QString voffset;
    double scaleValue;

    void initWidget();
    void initModelSetWidget();
    void initOptionSetWidget();
    void initModelTestWidget();
    void initConfig();
    void initSettingByConfig();
    void modifiedConfigJsonWakeupWord(QString word);
    void modifiedConfigJsonAutoRunValue(int value);
    void modifiedConfigJsonVoiceEnableValue(int value);
    void modifiedConfigJsonModelNameValue(QString name);
    void modifiedConfigJsonModelhoffsetValue(QString Modelhoffset);
    void modifiedConfigJsonModelvoffsetValue(QString Modelvoffset);
    void modifiedConfigJsonModelScaleValue(double value);
    void modifiedConfigJsonFile(QByteArray json);

    void saveApplicationRequest(const QString &AppPath);

    void AddConfigJSonRunApp(const QPair<QString, QString> &AppInfo);
    void refreshappNamePathMap();
signals:
    void modelChange(QString);
    void modelTestPath(QString);

    void wakeupwordChange(QString);
    void voiceEnableStateChange(int);

    void appNamePathMapChange(QMap<QString,QString>);
public slots:
    void currentModelchange_slot(QString);
    void modelHoffsetChange_slot(QString);
    void modelVoffsetChange_slot(QString);
    void modelScaleChange_slot(double value);

private slots:
    void autoRunCheckBox_stateChanged_slot(int value);
    void voiceEnableCheckBox_stateChanged_slot(int value);
    void wakeupWordLineEdit_trigger_slot(bool);

    void AddApplicationBt_dropUrlsChange_slot(QList<QUrl> urls);
    void AddApplicationInfo_slot(QPair<QString, QString> AppInfo);
};

#endif // SETTINGSWIDGET_H
