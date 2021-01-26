#ifndef LIVE2DWEBVIEW_H
#define LIVE2DWEBVIEW_H
#pragma comment(lib, "user32.lib")
#include <Windows.h>

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QWebEngineView>
#include <QJsonArray>
#include <QPointer>
#include <QMap>

#include <QMouseEvent>



class Live2dWebView : public QWebEngineView
{
    Q_OBJECT


public:
    Live2dWebView(QWidget *parent = nullptr);
    ~Live2dWebView();

    QString makeHtml(QString modelname);
    void setPosition(QString Position);

    void setHorizonOffset(QString Hoffset);
    void setVerticalOffset(QString Voffset);
    void setHorizonOffsetValue(QString Hoffset);
    void setVerticalOffsetValue(QString);

    QPoint getPoint(QPoint point);
    QString getModelPathFromModelName(QString Modelname);
    QMap<QString, QJsonObject> getModelMap();
    int getHorizonOffset();
    int getPosition();
    int getVerticalOffset();
    QString getModelName();

    QRect getModelRectWidget();
    void setModelScale(double value);
    void setModelScaleValue(double value);
    double getModelScale();



    void setCatModel(QString ModelName);
    void setCatHorizonOffsetValue(QString Hoffset);
    void setCatVerticalOffsetValue(QString Voffset);
public slots:
    void setModel(QString ModelName);
    void sendMouseByCursorPoint(QPoint point);
    void modelTestByModelPath(QString ModelPath);
protected:
    bool event(QEvent *event);
private:
    QObject* delegateWidget;

    bool WindowCrossFlag;
    LONG m_intExTemp;

    //Model Arg
    QString modelname;
    QString position;
    QString hoffset;
    QString voffset;
    int hoffsetValue;
    int voffsetValue;

    //max 2.2 min 0.5
    double ModelScale;

    //Cat Model
    QString cathoffset;
    QString catvoffset;

    QMap<QString,QJsonObject> modelMap;
    bool modelArrayflag;

    QString boardWidth;
    QWidget *rectwidget;
    QRect modelRect;

    void initWidget();
    void initWindowCrossFlag();
    void loadModelJson();


    QString makeCatHtml(QString Modelname);
signals:
    void currentModelchange(QString);
    void modelHoffsetChange(QString);
    void modelVoffsetChange(QString);
    void modelScaleChange(double);
};

#endif // LIVE2DWEBVIEW_H
