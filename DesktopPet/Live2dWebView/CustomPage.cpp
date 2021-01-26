#include "CustomPage.h"
#include <QDebug>

bool CustomPage::event(QEvent *event)
{
    qDebug()<<"page"<<event->type();

    return QWebEnginePage::event(event);
}

bool CustomPage::eventFilter(QObject *watched, QEvent *event)
{
    qDebug()<<watched<<event->type();
    return QWebEnginePage::eventFilter(watched,event);
}

CustomPage::CustomPage(QObject *parent) : QWebEnginePage(parent)
{
}

void CustomPage::setHtml(const QString &html, const QUrl &baseUrl)
{
    QWebEnginePage::setHtml(html,baseUrl);
    this->installEventFilter(this->view());
//    this->
}



