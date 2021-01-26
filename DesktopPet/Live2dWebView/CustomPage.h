#ifndef CUSTOMPAGE_H
#define CUSTOMPAGE_H

#include <QWebEnginePage>
#include <QEvent>

class CustomPage : public QWebEnginePage
{
    Q_OBJECT

protected:
    bool event(QEvent *) override;

    bool eventFilter(QObject *watched, QEvent *event) override;
public:
    explicit CustomPage(QObject *parent = nullptr);

    void setHtml(const QString &html, const QUrl &baseUrl = QUrl());
signals:

public slots:
};

#endif // CUSTOMPAGE_H
