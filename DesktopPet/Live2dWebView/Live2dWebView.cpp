#include "Live2dWebView.h"
#include <QChildEvent>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "CustomPage.h"

Live2dWebView::Live2dWebView(QWidget *parent)
    : QWebEngineView(parent)
    ,delegateWidget(nullptr),position("left")
    ,hoffset("0"),voffset("0"),hoffsetValue(0),voffsetValue(0),ModelScale(0.5)
    ,modelArrayflag(false),boardWidth("200")
{

    initWidget();

    //InitWindow Flags
    initWindowCrossFlag();

    loadModelJson();
}

Live2dWebView::~Live2dWebView()
{
}

QRect Live2dWebView::getModelRectWidget()
{
    return modelRect;
}

void Live2dWebView::setModelScale(double value)
{
    setModelScaleValue(value);

    this->setModel(modelname);

    emit(modelScaleChange(ModelScale));
}
void Live2dWebView::setModelScaleValue(double value)
{
    if(value < 0.5)
        ModelScale = 0.5;
    else if(value > 2.2)
        ModelScale = 2.2;
    else
        ModelScale = value;
}
double Live2dWebView::getModelScale()
{
    return ModelScale;
}

void Live2dWebView::setHorizonOffsetValue(QString Hoffset)
{
    if(Hoffset.isEmpty())
        return;

    hoffset = Hoffset;
    hoffsetValue = hoffset.toInt();
}
void Live2dWebView::setVerticalOffsetValue(QString Voffset)
{
    if(Voffset.isEmpty())
        return;

    voffset = Voffset;
    voffsetValue = voffset.toInt();
}
void Live2dWebView::setHorizonOffset(QString Hoffset)
{
    if(Hoffset.isEmpty())
        return;

    hoffset = Hoffset;
    hoffsetValue = hoffset.toInt();
    this->setModel(modelname);

    emit(modelHoffsetChange(hoffset));
}
void Live2dWebView::setVerticalOffset(QString Voffset)
{
    if(Voffset.isEmpty())
        return;

    voffset = Voffset;
    voffsetValue = voffset.toInt();
    this->setModel(modelname);

    emit(modelVoffsetChange(voffset));
}
void Live2dWebView::setPosition(QString Position)
{
    if(Position.isEmpty())
        return;

    position = Position;
    this->setModel(modelname);
}
void Live2dWebView::setModel(QString ModelName)
{
    if(ModelName.isEmpty())
        return;

    this->setHtml(makeHtml(ModelName));
    emit(currentModelchange(ModelName));
}
//CatModel Parameter
void Live2dWebView::setCatModel(QString ModelName)
{
    if(ModelName.isEmpty())
        return;

    this->setHtml(makeCatHtml(ModelName));
}
void Live2dWebView::setCatHorizonOffsetValue(QString Hoffset)
{
    if(Hoffset.isEmpty())
        return;

    cathoffset = Hoffset;
}
void Live2dWebView::setCatVerticalOffsetValue(QString Voffset)
{
    if(Voffset.isEmpty())
        return;

    catvoffset = Voffset;
}

int Live2dWebView::getHorizonOffset()
{
    return hoffsetValue;
}
int Live2dWebView::getVerticalOffset()
{
    return voffsetValue;
}

QString Live2dWebView::getModelName()
{
    return modelname;
}
int Live2dWebView::getPosition()
{
    int value;
    value = (QString::compare(position,"left") == 0) ? 1 : 2;
    return value;
}


void Live2dWebView::sendMouseByCursorPoint(QPoint point)
{
    if(delegateWidget != nullptr)
    {
        Qt::MouseButtons Buttons;
        Qt::KeyboardModifiers Modifiers;
        QMouseEvent *WebMouse = new QMouseEvent(QEvent::MouseMove,point,
                                                Qt::MouseButton::NoButton,Buttons,Modifiers);
        QApplication::sendEvent(delegateWidget,WebMouse);

        QString halfWidth = QString("%1").arg(boardWidth.toInt()*ModelScale);
        QString halfheight = QString("%1").arg(halfWidth.toInt() * 1.5);

        int boardwidthValue = halfWidth.toInt() * 2;
        int boardheightValue = halfheight.toInt() * 2;

        QRect rect = this->window()->geometry();
        if(QString::compare(position,"left") ==0)
            modelRect = QRect(0 + hoffsetValue,rect.height() - boardheightValue - voffsetValue,
                                boardwidthValue,boardheightValue);
        else if(QString::compare(position,"right") ==0)
            modelRect = QRect(rect.width() -boardwidthValue - hoffsetValue
                              ,rect.height() - boardheightValue - voffsetValue,
                              boardwidthValue,boardheightValue);


//        rectwidget->setGeometry(modelRect);
//        if(!rectwidget->isVisible())
//            rectwidget->show();

        if(modelRect.contains(point) && WindowCrossFlag)
        {
            SetWindowLong((HWND)this->window()->winId(), GWL_EXSTYLE, m_intExTemp);
            WindowCrossFlag = false;
        }
        else if(!modelRect.contains(point) && !WindowCrossFlag)
        {
            SetWindowLong((HWND)this->window()->winId(), GWL_EXSTYLE, m_intExTemp |WS_EX_TRANSPARENT | WS_EX_LAYERED);
            WindowCrossFlag = true;
        }

    }
}

QString Live2dWebView::getModelPathFromModelName(QString Modelname)
{
    if(modelArrayflag)
    {
        if(modelMap.contains(Modelname))
        {
            QJsonObject obj = modelMap.value(Modelname);
            return obj["modelpath"].toString();
        }
    }
    return QString();
}

QMap<QString,QJsonObject> Live2dWebView::getModelMap()
{
    if(modelArrayflag)
        return modelMap;
    else
        return QMap<QString,QJsonObject>();
}

bool Live2dWebView::event(QEvent *event)
{
    if(event->type() == QEvent::ChildAdded)
    {
        QChildEvent *child_ev = static_cast<QChildEvent*>(event);
        delegateWidget = child_ev->child();
    }
    return QWebEngineView::event(event);
}


void Live2dWebView::initWidget()
{
    rectwidget = new QWidget(this);
    rectwidget->setWindowFlags(Qt::Tool|Qt::FramelessWindowHint);
    rectwidget->setWindowOpacity(0.2);
    rectwidget->setStyleSheet("border:1px solid black;");
    rectwidget->hide();
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setContextMenuPolicy(Qt::NoContextMenu);
    this->page()->setBackgroundColor(Qt::transparent);
}

void Live2dWebView::initWindowCrossFlag()
{
    m_intExTemp = GetWindowLong((HWND)this->window()->winId(), GWL_EXSTYLE);

    SetWindowLong((HWND)this->window()->winId(), GWL_EXSTYLE, m_intExTemp |WS_EX_TRANSPARENT | WS_EX_LAYERED);
    WindowCrossFlag = true;
}

void Live2dWebView::loadModelJson()
{
    QFile file(":/res/file/modelPath.json");
    if(!file.exists())
        return;
    file.open(QIODevice::ReadOnly|QIODevice::Text);

    QString value = file.readAll();
    file.close();

    QJsonParseError pareseError;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&pareseError);
    if(!(pareseError.error == QJsonParseError::NoError))
        return;

    QJsonObject jsonObject = document.object();
    if(jsonObject.contains(QStringLiteral("model")))
    {
        QJsonValue modelValue = jsonObject.value(QStringLiteral("model"));
        if(modelValue.isArray())
        {
            modelArrayflag = true;
            QJsonArray modelArray = modelValue.toArray();

            for(int i=0;i<modelArray.size();i++)
            {
                QJsonValue value = modelArray.at(i);
                QJsonObject obj = value.toObject();
                modelMap.insert(obj["name"].toString(),obj);
            }
        }
    }

}



QString Live2dWebView::makeHtml(QString Modelname)
{
    QString halfWidth = QString("%1").arg(boardWidth.toInt()*ModelScale);
    QString halfheight = QString("%1").arg(halfWidth.toInt() * 1.5);

    QString modelPath = getModelPathFromModelName(Modelname);
    if(modelPath.isEmpty())
        return QString();

    modelname = Modelname;
    QString modelConfig;

    modelConfig = "'model': {"
                  "'jsonPath': \"" +modelPath+"\","
                  "},"
                  "'display': {"
                    "'superSample': 2,"
                    "'width': "+halfWidth+","
                    "'height': "+halfheight+","
                    "'position' : \""+position+"\","
                    "'hOffset': "+hoffset+","
                    "'vOffset': "+voffset+
                  "},"
                  "'react': {"
                    "'opacityDefault': 1,"
                    "'opacityOnHover': 1"
                  "}";
//            "'dev':{"
//                  "'border':'true'"
//                  "}";

    QString html = ""
"<!DOCTYPE html>"
  "<html>"
    "<head>"
      "<script src =\"https://cdn.jsdelivr.net/gh/WSRD-Gauss/desktopPetWebJS/lib/L2Dwidget.min.js\"></script>"
    "</head>"
    "<body>"
    "<script type=\"text/javascript\">"
      "var currL2Dwidget = new L2Dwidget.init({"
        + modelConfig +
      "});"
    "</script>"
  "</body>"
"</html>";
    return html;
}
QString Live2dWebView::makeCatHtml(QString Modelname)
{
    QString halfWidth = QString("%1").arg(boardWidth.toInt()*0.5);
    QString halfheight = QString("%1").arg(halfWidth.toInt() * 1.2);

    QString modelPath = getModelPathFromModelName(Modelname);
    if(modelPath.isEmpty())
        return QString();

    QString modelConfig;

    modelConfig = "'model': {"
                  "'jsonPath': \"" +modelPath+"\","
                  "},"
                  "'display': {"
                    "'superSample': 2,"
                    "'width': "+halfWidth+","
                    "'height': "+halfheight+","
                    "'position' : \""+position+"\","
                    "'hOffset': "+cathoffset+","
                    "'vOffset': "+catvoffset+
                  "},"
                  "'react': {"
                    "'opacityDefault': 1,"
                    "'opacityOnHover': 1"
                  "}";

    QString html = ""
"<!DOCTYPE html>"
  "<html>"
    "<head>"
      "<script src =\"https://cdn.jsdelivr.net/gh/WSRD-Gauss/desktopPetWebJS/lib/L2Dwidget.min.js\"></script>"
    "</head>"
    "<body>"
    "<script type=\"text/javascript\">"
      "var currL2Dwidget = new L2Dwidget.init({"
        + modelConfig +
      "});"
    "</script>"
  "</body>"
"</html>";
    return html;
}
void Live2dWebView::modelTestByModelPath(QString ModelPath)
{
    qDebug()<<"Modelpath"<<ModelPath;
    QString halfWidth = QString("%1").arg(boardWidth.toInt()/2);
    QString halfheight = QString("%1").arg(boardWidth.toInt() * 1.2);
    QString modelConfig;

    modelConfig = "'model': {"
                  "'jsonPath': \"" +ModelPath+"\","
                  "},"
                  "'display': {"
                    "'superSample': 2,"
                    "'width': "+halfWidth+","
                    "'height': "+halfheight+","
                    "'position' : \""+position+"\","
                    "'hOffset': "+hoffset+","
                    "'vOffset': "+voffset+
                  "},"
                  "'react': {"
                    "'opacityDefault': 1,"
                    "'opacityOnHover': 1"
                  "},";
//            "'dev':{"
//                  "'border':'true'"
//                  "}"

    QString html = ""
"<!DOCTYPE html>"
  "<html>"
    "<head>"
      "<script src =\"https://cdn.jsdelivr.net/gh/WSRD-Gauss/desktopPetWebJS/lib/L2Dwidget.min.js\"></script>"
    "</head>"
    "<body>"
    "<script type=\"text/javascript\">"
      "var currL2Dwidget = new L2Dwidget.init({"
        + modelConfig +
      "});"
    "</script>"
  "</body>"
"</html>";


    this->setHtml(html);
}
