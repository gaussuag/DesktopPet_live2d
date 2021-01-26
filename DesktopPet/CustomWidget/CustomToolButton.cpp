#include "CustomToolButton.h"
#include <QDebug>


CustomToolButton::CustomToolButton(QWidget *parent) : QToolButton(parent)
{

}
CustomToolButton::CustomToolButton(const QString& text, QWidget *parent):QToolButton(parent)
{
	this->setText(text);
}

CustomToolButton::~CustomToolButton()
{

}

void CustomToolButton::setCenterAlignment()
{
    _CenterAlignment = true;
	_FirstSetAlignmentFlag = true;
}
void CustomToolButton::setRotate(qreal rotate)
{
    if(!text().isEmpty() && !_Transform)
    {
        _Text = text();
        setText("");
    }
    if(!_Transform)
        _Transform = true;
    _Rotate = rotate;
    update();
}

void  CustomToolButton::setText(const QString &text)
{
    if(_Transform)
        _Text = text;
    else
        return QToolButton::setText(text);
}
QString CustomToolButton::text()
{
    if(_Transform)
        return _Text;
    else
        return QToolButton::text();
}

void CustomToolButton::dragEnterEvent(QDragEnterEvent *event)
{
    if(acceptDrops())
        event->acceptProposedAction();
}

void CustomToolButton::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(!urls.isEmpty())
        emit(dropUrlsChange(urls));
}

bool CustomToolButton::event(QEvent *e)
{
    if(e->type() == QEvent::Resize && _CenterAlignment)
        updateCenterAlignmentStylesheet();
    return QToolButton::event(e);
}

void CustomToolButton::paintEvent(QPaintEvent *event)
{
    QToolButton::paintEvent(event);
    if(_Transform)
        DrawRotateText();

	
}
void CustomToolButton::DrawRotateText()
{
    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QFont f = painter.font();
    QFontMetrics fm(f);
    QRectF br(fm.boundingRect(_Text));

	QPainterPath path;
	path.addText(0, 0, f, _Text);

    QPointF rectCenter = this->rect().center();
    QPointF center(br.center());

    //平移原点位置，根据矩形中心点和文字矩形中心点进行平移计算
    painter.translate(rectCenter-center);
    painter.translate(center);
    painter.rotate(_Rotate);
    painter.translate(-center);

	QColor color = this->palette().buttonText().color();
    painter.fillPath(path, color);

    painter.restore();
}


void CustomToolButton::updateCenterAlignmentStylesheet()
{
    QFontMetrics fontMetrics = this->fontMetrics();
    int CenterWidth = fontMetrics.width(this->text()) + this->iconSize().width();
    int Width = this->width();
    int padding_left = (Width - CenterWidth)/2;

    QString originSheet = this->styleSheet();

	if(_FirstSetAlignmentFlag)
	{
		originSheet.append(QString("QToolButton{padding-left:%1}"
			"QToolButton::pressed{padding-left:%2;padding-top:1;}")
			.arg(padding_left).arg(padding_left + 2));
		_FirstSetAlignmentFlag = false;
	}else
	{
		originSheet.replace(QRegExp("\\bQToolButton\\{padding\\-left\\:\\d*\\}"), QString("QToolButton{padding-left:%1}").arg(padding_left));
		originSheet.replace(QRegExp("\\bQToolButton\\:\\:pressed\\{padding\\-left\\:\\d*\\;\\b"), QString("QToolButton::pressed{padding-left:%2;").arg(padding_left + 2));
	}



    this->setStyleSheet(originSheet);

}
void CustomToolButton::setTransparentStyleSheet()
{
	QString originSheet = this->styleSheet();

	originSheet.append("QToolButton{background-color:transparent;}"
		"QToolButton::hover{background-color:gray;color:white;border-radius:2px;}"
		"QToolButton::pressed{background-color:gray;padding-left:2px;padding-top:2px;}");

	this->setStyleSheet(originSheet);
}
