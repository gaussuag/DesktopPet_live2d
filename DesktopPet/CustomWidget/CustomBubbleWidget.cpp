#include "CustomBubbleWidget.h"
#include <QApplication>
#include <QDesktopWidget>


CustomBubbleWidget::CustomBubbleWidget(QWidget *parent)
    :QWidget(parent)
{
    initWidget();
}

CustomBubbleWidget::~CustomBubbleWidget()
{

}
void CustomBubbleWidget::initWidget()
{
    _DesktopRect = QApplication::desktop()->availableGeometry();
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
}

void CustomBubbleWidget::setCenterWidget(QWidget *widget)
{
    _CenterWidget = widget;

    _CenterWidgetLayout = new QHBoxLayout(this);
    _CenterWidgetLayout->addWidget(widget);
    _CenterWidgetLayout->setSpacing(0);
    setCenterWidgetLayoutContentsMargins();
}
QWidget * CustomBubbleWidget::getCenterWidget()
{
	return _CenterWidget;
}
void CustomBubbleWidget::setCenterWidgetLayoutContentsMargins()
{
    //advance handle Parameter
    handleDrawParameter();
    qreal FilletRadius = ( (this->width() < this->height()) ? this->width() : this->height() ) /8;



    switch (_arrowDirection) {
    case Top_Left:
    case Top_Right:
    case Top_Center:
        _LayoutMargin = QMargins(FilletRadius,FilletRadius+_triangleHeight,FilletRadius,FilletRadius);
        break;
    case Bottom_Left:
    case Bottom_Right:
    case Bottom_Center:
        _LayoutMargin = QMargins(FilletRadius,FilletRadius,FilletRadius,FilletRadius + _triangleHeight);
        break;
    case Left:
        _LayoutMargin = QMargins(FilletRadius+_triangleHeight,FilletRadius,FilletRadius,FilletRadius);
        break;
    case Right:
        _LayoutMargin = QMargins(FilletRadius,FilletRadius,FilletRadius+_triangleHeight,FilletRadius);
        break;
    default:
        break;
    }
    if(_CenterWidgetLayout != nullptr)
        _CenterWidgetLayout->setContentsMargins(_LayoutMargin);
}

void CustomBubbleWidget::setArrowDirection(CustomBubbleWidget::ArrowDirection direction)
{
    _arrowDirection = direction;
    setCenterWidgetLayoutContentsMargins();
    this->update();
}

void CustomBubbleWidget::setBackgroundColor(QColor color)
{
    _BackgroundColor = color;
    _BackgroundColor = _BackgroundColor;
    this->update();
}

void CustomBubbleWidget::show()
{
    handlePositionOnArrow();
    QWidget::show();
}
void CustomBubbleWidget::handlePositionOnArrow()
{
    //Advance HandleParameter
    handleDrawParameter();
    getTriangPolygon();

    QPoint currentPos = this->pos();

    switch(_arrowDirection)
    {
        case Top_Left:
        case Top_Right:
        case Top_Center:
            currentPos.setX(currentPos.x() - _ArrowTopPoint.x());
            break;
        case Bottom_Left:
        case Bottom_Right:
        case Bottom_Center:
            currentPos.setX(currentPos.x() - _ArrowTopPoint.x());
            currentPos.setY(currentPos.y() - _ArrowTopPoint.y());
            break;
        case Left:
            currentPos.setY(currentPos.y() - _ArrowTopPoint.y());
            break;
        case Right:
            currentPos.setX(currentPos.x() - _ArrowTopPoint.x());
            currentPos.setY(currentPos.y() - _ArrowTopPoint.y());
            break;
    }
    this->setGeometry(currentPos.x(),currentPos.y(),this->size().width(),this->size().height());
}


void CustomBubbleWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    paintWidget(&painter);

}
void CustomBubbleWidget::paintWidget(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing,true);

    handleDrawParameter();

    //Draw RoundRect
    DrawRoundRect(painter);

    //DrawTriang
    DrawTriang(painter);

    qDebug()<<"draw";
}
void CustomBubbleWidget::DrawRoundRect(QPainter *painter)
{
    _Rect = getDrawRect();
    painter->setPen(Qt::NoPen);
    painter->setBrush(_BackgroundColor);

    qreal referenceArg = (_Rect.width() < _Rect.height()) ? _Rect.width() : _Rect.height();
    painter->drawRoundedRect(_Rect,referenceArg/4,referenceArg/4);
}
void CustomBubbleWidget::DrawTriang(QPainter *painter)
{
    QPolygon TriangPolygon = getTriangPolygon();
    painter->drawPolygon(TriangPolygon);
}

void CustomBubbleWidget::handleDrawParameter()
{
    //handleTriangParameter
    QRect widgetRect = this->rect();
    qreal referenceArg;
    if((_arrowDirection == Left) || (_arrowDirection == Right))
    {
        referenceArg = (widgetRect.width() <= widgetRect.height()) ? widgetRect.width() : widgetRect.height();
        _triangleHeight = referenceArg / 4;
        _triangleWidth =  referenceArg / 3;
    }
    else
    {
        referenceArg = widgetRect.width();
        _triangleoffset = referenceArg / 6;
        _triangleHeight = referenceArg / 30;
        _triangleWidth =  referenceArg / 15;
    }
}

QRectF CustomBubbleWidget::getDrawRect()
{
    QRect widgetRect = this->rect();
    switch (_arrowDirection) {
    case Top_Left:
    case Top_Right:
    case Top_Center:
            return QRectF(0,_triangleHeight,widgetRect.width()-1,widgetRect.height() - _triangleHeight -1);
    case Bottom_Left:
    case Bottom_Right:
    case Bottom_Center:
            return QRectF(0,0,widgetRect.width()-1,widgetRect.height()- _triangleHeight -1);
    case Left:
            return QRectF(_triangleHeight,0,widgetRect.width() - _triangleHeight -1,widgetRect.height() -1);
    case Right:
            return QRectF(0,0,widgetRect.width() - _triangleHeight -1,widgetRect.height() -1);
    default:
        return QRectF();
    }
}
QPolygon CustomBubbleWidget::getTriangPolygon()
{
    QRectF rect = getDrawRect();
    QPolygon polygon;
    switch(_arrowDirection)
    {
        case Top_Left:
            polygon << QPoint(_triangleoffset,_triangleHeight);
            polygon << QPoint(_triangleoffset+_triangleWidth/2,0);
            _ArrowTopPoint = QPoint(_triangleoffset+_triangleWidth/2,0);
            polygon << QPoint(_triangleoffset+_triangleWidth,_triangleHeight);
            break;
        case Top_Right:
            polygon << QPoint(rect.width() - _triangleWidth - _triangleoffset,_triangleHeight);
            polygon << QPoint(rect.width() - _triangleWidth/2 - _triangleoffset,0);
            _ArrowTopPoint = QPoint(rect.width() - _triangleWidth/2 - _triangleoffset,0);
            polygon << QPoint(rect.width() - _triangleoffset,_triangleHeight);
            break;
        case Top_Center:
            polygon << QPoint(rect.width()/2 - _triangleWidth/2,_triangleHeight);
            polygon << QPoint(rect.width()/2,0);
            _ArrowTopPoint = QPoint(rect.width()/2,0);
            polygon << QPoint(rect.width()/2 + _triangleWidth/2,_triangleHeight);
            break;
        case Bottom_Left:
            polygon << QPoint(_triangleoffset,rect.height());
            polygon << QPoint(_triangleoffset+_triangleWidth/2,rect.height() + _triangleHeight);
            _ArrowTopPoint = QPoint(_triangleoffset+_triangleWidth/2,rect.height() + _triangleHeight);
            polygon << QPoint(_triangleoffset+_triangleWidth,rect.height());
            break;
        case Bottom_Right:
            polygon << QPoint(rect.width() - _triangleWidth - _triangleoffset,rect.height());
            polygon << QPoint(rect.width() - _triangleWidth/2 - _triangleoffset,rect.height() + _triangleHeight);
            _ArrowTopPoint = QPoint(rect.width() - _triangleWidth/2 - _triangleoffset,rect.height() + _triangleHeight);
            polygon << QPoint(rect.width() - _triangleoffset,rect.height());
            break;
        case Bottom_Center:
            polygon << QPoint(rect.width()/2 - _triangleWidth/2,rect.height());
            polygon << QPoint(rect.width()/2,rect.height() + _triangleHeight);
            _ArrowTopPoint = QPoint(rect.width()/2,rect.height() + _triangleHeight);
            polygon << QPoint(rect.width()/2 + _triangleWidth/2,rect.height());
            break;
        case Left:
            polygon << QPoint(_triangleHeight,rect.height()/2 - _triangleWidth/2);
            polygon << QPoint(0,rect.height()/2);
            _ArrowTopPoint = QPoint(0,rect.height()/2);
            polygon << QPoint(_triangleHeight,rect.height()/2 + _triangleWidth/2);
            break;
        case Right:
            polygon << QPoint(rect.width(),rect.height()/2 - _triangleWidth/2);
            polygon << QPoint(rect.width() + _triangleHeight,rect.height()/2);
            _ArrowTopPoint = QPoint(rect.width() + _triangleHeight,rect.height()/2);
            polygon << QPoint(rect.width(),rect.height()/2 + _triangleWidth/2);
            break;
    }
    return polygon;
}

