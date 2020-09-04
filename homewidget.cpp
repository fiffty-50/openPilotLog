/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
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
//    ui->debugLineEdit->setText(dbAircraft::retreiveAircraftDetails("102")[1]);
    qDebug() << "Debug: ";
    dbPilots::retreivePilotList();
}
