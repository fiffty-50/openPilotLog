#include "homewidget.h"
#include "ui_homewidget.h"

#include <QDebug>

homeWidget::homeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::homeWidget)
{
    ui->setupUi(this);
    qDebug() << "homeWidget: Activated";
    ui->backgroundLabel->clear();

    //QPixmap pix("://background_gradient.png");
    //ui->backgroundLabel->setPixmap(pix);
    ui->backgroundLabel->setStyleSheet("background-color: rgba(13, 196, 254, 100);");
}

homeWidget::~homeWidget()
{
    delete ui;
}
