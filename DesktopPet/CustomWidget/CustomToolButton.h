#ifndef CUSTOMTOOLBUTTON_H
#define CUSTOMTOOLBUTTON_H

#include <QToolButton>
#include <QMimeData>
#include <QEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>

#include <QUrl>


class CustomToolButton : public QToolButton
{
    Q_OBJECT
public:
    explicit CustomToolButton(QWidget *parent = 0);
	explicit CustomToolButton(const QString& text,QWidget *parent = 0);
    ~CustomToolButton();

    void setCenterAlignment();
    void setTransparentStyleSheet();
    void setText(const QString &text);
    QString text();
    void setRotate(qreal rotate);

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);

    bool event(QEvent *e);
	void paintEvent(QPaintEvent *);
    void DrawRotateText();
private:
    bool _CenterAlignment = false;
	bool _FirstSetAlignmentFlag = false;
    bool _Transform = false;
    qreal _Rotate = 0;
    QString _Text;

    void updateCenterAlignmentStylesheet();
	
signals:
    void dropUrlsChange(QList<QUrl>);
};

#endif // CUSTOMTOOLBUTTON_H
