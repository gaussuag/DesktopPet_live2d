#include "ModelSetWidget.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListView>
#include <QStandardItemModel>

#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkReply>
#include <QPixmap>

#include <QDebug>

#include "CustomToolButton.h"

ModelSetWidget::ModelSetWidget(QWidget *parent) : QWidget(parent)
{
    initWidget();
    initModelPreViewWidget();
}

ModelSetWidget::~ModelSetWidget()
{

}

void ModelSetWidget::setModelMap(QMap<QString, QJsonObject> map)
{
    modelmap = map;
    if(!map.isEmpty())
        setListModel();
}

QMap<QString, QJsonObject> ModelSetWidget::getModelMap()
{
    return modelmap;
}

void ModelSetWidget::setCurrentModelPreViewLabel(QString name)
{
    if(modelmap.contains(name))
    {
        QString modelPng = modelmap.value(name)["previewpath"].toString();
        //modelLabel
        ModelPreViewLabel->setPixmap(QPixmap(modelPng).scaled(ModelPreViewLabel->width()
                                                              ,ModelPreViewLabel->height()
                                                              ,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    }
}
void ModelSetWidget::initWidget()
{
    setAttribute(Qt::WA_StyledBackground);

    QHBoxLayout *HBoxLayout = new QHBoxLayout(this);
    CustomToolButton *SwitchButton = new CustomToolButton(QString::fromLocal8Bit("ÇÐ»»Ä£ÐÍ"),this);
    SwitchButton->setMinimumSize(100,40);
    SwitchButton->setStyleSheet("CustomToolButton{background-color: rgb(221, 255, 255);"
                                 "border:1px solid rgb(221,221,221);"
                                 "border-radius:10px;"
                                 "font: 10pt;}"
                                 "CustomToolButton::hover{background-color: rgb(180, 255, 255);}");

    connect(SwitchButton,SIGNAL(pressed()),this,SLOT(SwitchButton_slot()));

    ModelPreViewLabel = new QLabel(this);
    ModelPreViewLabel->setMinimumSize(250,250);
    ModelPreViewLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    HBoxLayout->addWidget(SwitchButton);
    HBoxLayout->addWidget(ModelPreViewLabel);
}

void ModelSetWidget::initModelPreViewWidget()
{
    previewModelWidget = new QWidget(this);
    previewModelWidget->setWindowFlag(Qt::Dialog);
    previewModelWidget->setFixedSize(650,400);
    previewModelWidget->hide();

    QHBoxLayout *HBoxLayout = new QHBoxLayout(previewModelWidget);
    modelLabel = new QLabel(previewModelWidget);
    modelLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    listView = new QListView(previewModelWidget);
    listView->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    listView->setMouseTracking(true);
    listView->setEditTriggers(QListView::NoEditTriggers);

    connect(listView, &QListView::entered, this, [=](const QModelIndex& index){
        setPreviewPixmap(index);
    });
    connect(listView,&QListView::doubleClicked,this,[=](const QModelIndex& index){
        emit(modelChange(standardModel->item(index.row())->text()));
    });

    standardModel = new QStandardItemModel(this);
    listView->setModel(standardModel);


    HBoxLayout->addWidget(modelLabel);
    HBoxLayout->addWidget(listView);
}

void ModelSetWidget::setListModel()
{
    QList<QString> List = modelmap.keys();
    for(const auto & name : List)
        standardModel->appendRow(new QStandardItem(name));
}
void ModelSetWidget::setPreviewPixmap(const QModelIndex& index)
{
    QString pngPath = modelmap.value(standardModel->item(index.row())->text())["previewpath"].toString();
    //modelLabel
    modelLabel->setPixmap(QPixmap(pngPath));
}

void ModelSetWidget::SwitchButton_slot()
{
    if(!previewModelWidget->isVisible())
        previewModelWidget->show();
}



