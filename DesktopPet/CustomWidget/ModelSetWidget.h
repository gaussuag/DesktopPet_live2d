#ifndef MODELSETWIDGET_H
#define MODELSETWIDGET_H

#include <QWidget>
#include <QMap>
#include <QJsonObject>

class QLabel;
class QListView;
class QStandardItemModel;

class ModelSetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModelSetWidget(QWidget *parent = nullptr);
    ~ModelSetWidget();
    void setModelMap(QMap<QString,QJsonObject> map);
    QMap<QString,QJsonObject> getModelMap();

    void setCurrentModelPreViewLabel(QString name);
private:
    QLabel *ModelPreViewLabel;

    QWidget *previewModelWidget;
    QLabel *modelLabel;
    QListView* listView;
    QStandardItemModel* standardModel;

    QMap<QString,QJsonObject> modelmap;

    void initWidget();
    void initModelPreViewWidget();
    void setListModel();
    void setPreviewPixmap(const QModelIndex &index);
signals:
    void modelChange(QString);
public slots:

private slots:
    void SwitchButton_slot();
};

#endif // MODELSETWIDGET_H
