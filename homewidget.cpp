#include "homewidget.h"
#include "ui_homewidget.h"
#include "calc.h"
#include "dbman.cpp"
#include "dbstat.h"

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

    ui->totalTimeDisplayLabel->setText(calc::minutes_to_string(dbStat::retreiveTotalTime()));

    QString blockMinutesThisYear = dbStat::retreiveTotalTimeThisCalendarYear();
    ui->blockHoursCalDisplayLabel->setText(calc::minutes_to_string(blockMinutesThisYear));
    if (blockMinutesThisYear.toInt() > 900*60) {
        qDebug() << "More than 900 block hours this calendar year!";
        // set Text Red
    }
    QString blockMinutesRollingYear = dbStat::retreiveTotalTimeRollingYear();
    ui->blockHoursRolDisplayLabel->setText(calc::minutes_to_string(blockMinutesRollingYear));
    QVector<QString> currency = dbStat::retreiveCurrencyTakeoffLanding();
    ui->currencyDisplayLabel->setText(currency[0] + " Take Offs\n" + currency[1] + " Landings");
    if (currency[0].toInt() < 3 || currency[1].toInt() < 3){
        qDebug() << "Less than 3 TO/LDG in last 90 days!";
        //set Text Red
    }
}

homeWidget::~homeWidget()
{
    delete ui;
}

void homeWidget::on_debugButton_clicked()
{
    ui->debugLineEdit->setText(dbStat::retreiveTotalTimeRollingYear());
}
