#ifndef DESKTOPPET_H
#define DESKTOPPET_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QEvent>
#include <QMouseEvent>
#include <QtConcurrent/QtConcurrent>
#include <QPoint>


class QLabel;
class QSlider;
class QVBoxLayout;
class QComboBox;
class QToolButton;
class Live2dWebView;
class SettingsWidget;
class CustomBubbleWidget;

class DesktopPet : public QWidget
{
    Q_OBJECT
public:
    DesktopPet(QWidget *parent = nullptr);
    ~DesktopPet();

    void setWatchedCursor(bool flag);

protected:
    bool eventFilter(QObject *w, QEvent *e);
private:
    QVBoxLayout *VBoxLayout;

    Live2dWebView *ModelView;

    bool watchedFlag;
    QFuture<void> ThreadRes;
    QPoint CurrentPoint;
    //
    QSystemTrayIcon *SysTrayIcon;

    SettingsWidget *settingsWidget;

    //adjust
    QAction *adjustAction;
    QWidget *adjustWidget;
    QSlider *horScroll;
    QSlider *vorScroll;
    QSlider *ScaleSlider;
    bool adjustFlag;

    //SupCatWidget
    QList<QPair<int,int>> EqulSortList;
    QWidget *supCatWidget;
    QComboBox *ChoiceCatNumBox;
    QComboBox *SortCatNumBox;
    QToolButton *ShowCatBt;
    QToolButton *HideCatBt;
    bool supCatFlag;
    QList<Live2dWebView *>supCatList;

    //BubbleWidget
    CustomBubbleWidget *BubbleWidget;
    QLabel *BubbleLabel;

    //Model Control
    bool wakeupflag;

    //System Exist
    QMap<QString,QString> SysAppMap;

    //Config
    int autoRun;
    int voiceEnable;
    QString wakeupword;
    QMap<QString,QString> appNamePathMap;
    QString currentmodelName;
    QString currentmodelhoffset;
    QString currentmodelvoffset;
    double currentmodescale;

    void initWidget();
    void initModelViewWidget();
    void initSysTrayIcon();
    void initSettingWidget();
    void initAdjustWidget();
    void initSupCatWidget();
    void initBubbleWidget();
    void initConnect();
    void initConfig();
    void initSysAppCode();

    void watchedCursor();

    void checkWakeupOption(const QString &);
    void handleModelCode(const QString &);
    bool checkOutOption(const QString &);
    bool checkCodeAboutRunApp(const QString &);
    void runSysAppByApplicationCode(const QString &AppName);
    void runAppByApplicationName(const QString &);
    bool checkModelAction(const QString &);
    void modelLookAround();
    void hideBubbleWidget();
private slots:
    void CursorPointChange_slot(QPoint);

    void setAction_slot();
    void adjustAction_slot();
    void SupCatAction_slot();
    void ShowCatBt_slot();
    void SnapAction_slot();
    void HideCatBt_slot();

    void wakeupwordChange_slot(QString word);
    void appNamePathMapChange_slot(QMap<QString, QString> AppNamePathMap);
signals:
    void CursorPointChange(QPoint);

};
#endif // DESKTOPPET_H
