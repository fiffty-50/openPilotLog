#include "homewidget.h"
#include "ui_homewidget.h"

#include <QDebug>

homeWidget::homeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::homeWidget)
{
    ui->setupUi(this);
    qDebug() << "homeWidget: Activated";


    /*
     * To Do: Functions to retreive values from DB
     */

    ui->totalTimeDisplayLabel->setText("123:45");
    ui->blockHoursDisplayLabel->setText("123:45");
    ui->currencyDisplayLabel->setText("17 Take Offs\n15 Landings");
}

homeWidget::~homeWidget()
{
    delete ui;
}
