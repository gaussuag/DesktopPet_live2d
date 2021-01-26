#ifndef SETAPPLICATIONINFODIALOG_H
#define SETAPPLICATIONINFODIALOG_H

#include <QDialog>
#include <QPair>
#include <QString>

class QLineEdit;

class SetApplicationInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetApplicationInfoDialog(QString AppPath, QWidget *parent = nullptr);
    ~SetApplicationInfoDialog();

private:
     QLineEdit *wakeupApplicationLineEdit;
     QString appPath;

     void initWidget();
signals:
    void AddApplicationInfo(QPair<QString,QString>);
public slots:
private slots:
     void ComfirmBt_pressed_slot();
};

#endif // SETAPPLICATIONINFODIALOG_H
