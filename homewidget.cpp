#include "homewidget.h"
#include "ui_homewidget.h"

#include <QDebug>

homeWidget::homeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::homeWidget)
{
    ui->setupUi(this);
    qDebug() << "homeWidget: Activated";
}

homeWidget::~homeWidget()
{
    delete ui;
}
