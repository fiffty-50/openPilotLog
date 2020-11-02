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

// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr


homeWidget::homeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::homeWidget)
{
    ui->setupUi(this);
    showTotals();
    /*ui->totalTimeThisYearDisplay->setText(
                calc::minutes_to_string(
                stat::totalTime(stat::calendarYear)));
    ui->totalTime365DaysDisplay->setText(
                calc::minutes_to_string(
                stat::totalTime(stat::rollingYear)));
    QVector<QString> toldg = stat::currencyTakeOffLanding(90);
    QString ToLdg;// = toldg[0] + " / " + toldg[1];
    for(const auto& item : toldg)
    {
        ToLdg += item;
        if(toldg.indexOf(item) != toldg.length()-1) {ToLdg += QLatin1String(" / ");}
    }*/

    //ui->ToLdgDisplay->setText(ToLdg);
}

homeWidget::~homeWidget()
{
    delete ui;
}

void homeWidget::on_pushButton_clicked()
{
    auto pl = new pilot("pilots",498);
    auto np = new NewPilot(*pl,db::editExisting,this);
    np->show();
}

void homeWidget::showTotals()
{
    auto tw = new totalsWidget(this);
    ui->stackedWidget->addWidget(tw);
    ui->stackedWidget->setCurrentWidget(tw);
    ui->stackedWidget->show();

}
