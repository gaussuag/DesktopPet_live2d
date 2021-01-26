#ifndef CUSTOMBUBBLEWIDGET_H
#define CUSTOMBUBBLEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QRect>
#include <QMargins>
#include <QDebug>
#include <QHBoxLayout>
#include <QTimer>
#include <QEvent>
#include <QStyleOption>



class CustomBubbleWidget : public QWidget
{
    Q_OBJECT
public:
    enum ArrowDirection {
            Top_Left = 1,
            Top_Right = 2,
            Top_Center = 3,
            Bottom_Left = 4,
            Bottom_Right = 5,
            Bottom_Center = 6,
            Left = 7,
            Right = 8
        };
    enum WidgetPopupDirection{
            PopupTop = 1,
            PopupBottom = 2,
            PopupLeft = 3,
            PopupRight = 4
    };


    explicit CustomBubbleWidget(QWidget *parent = 0);
    ~CustomBubbleWidget();


	
    void setBackgroundColor(QColor color);

    void setCenterWidget(QWidget *widget);
	QWidget *getCenterWidget();
    void setArrowDirection(ArrowDirection direction);




    void show();
signals:
    void widgetshow(QWidget *);

protected:
    void paintEvent(QPaintEvent *event);

    void paintWidget(QPainter *painter);
    void DrawRoundRect(QPainter *painter);
    void DrawTriang(QPainter *painter);
private:
    ArrowDirection _arrowDirection = Left;
    WidgetPopupDirection _PopupDirection  = PopupBottom;


    QMargins _LayoutMargin;
    QHBoxLayout *_CenterWidgetLayout=nullptr;
    QWidget *_CenterWidget=nullptr;

    QColor _BackgroundColor=Qt::gray;

    int _triangleHeight;
    int _triangleWidth;
    int _triangleoffset;
    QRect _DesktopRect;
    QRectF _Rect;
    QPointF _ArrowTopPoint;

    void setCenterWidgetLayoutContentsMargins();


    void initWidget();
    QRectF getDrawRect();
    QPolygon getTriangPolygon();
    void handleDrawParameter();
    void handlePositionOnArrow();
    void StrongFresh();
};

#endif // CUSTOMBUBBLEWIDGET_H
