#include "homewidget.h"
#include "ui_homewidget.h"

#include <QDebug>

homeWidget::homeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::homeWidget)
{
    ui->setupUi(this);
    qDebug() << "homeWidget: Activated";

    //ui->backgroundLabel->clear();
    //QPixmap pix("://background_gradient.png");
    //ui->backgroundLabel->setPixmap(pix);
    //ui->backgroundLabel->setStyleSheet("background-color: rgba(13, 196, 254, 20);");
    /*
     * To Do: Functions to retreive values from DB
     */

    ui->totalTimeDisplayLabel->setText("123:45");
    ui->blockHoursDisplayLabel->setText("123:45");
    ui->currencyDisplayLabel->setText("17 Take Offs and 15 Landings in the last 90 days.");
}

homeWidget::~homeWidget()
{
    delete ui;
}
