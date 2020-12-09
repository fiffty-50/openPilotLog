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
#include "debug.h"


HomeWidget::HomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeWidget)
{
    ui->setupUi(this);
    totalsWidget = new TotalsWidget(this);
    ui->stackedWidget->addWidget(totalsWidget);
    ui->stackedWidget->setCurrentWidget(totalsWidget);
    ui->stackedWidget->show();
}

HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::on_pushButton_clicked()
{
    using namespace experimental;
    DB()->connect();
    DataPosition dp = {"pilots", 7};
    DEB(DB()->getEntryData(dp));
    /*long intloop = 0;
    long rangeloop = 0;
    for (int i = 10; i < 100; i++) {
        DataPosition dp = {"pilots", i};

        auto start = std::chrono::system_clock::now();
        DB()->getEntryData(dp);
        auto stop = std::chrono::system_clock::now();
        auto duration = stop - start;
        intloop += duration.count();
        auto start2 = std::chrono::system_clock::now();
        DB()->getEntryDataFor(dp);
        auto stop2 = std::chrono::system_clock::now();
        auto duration2 = stop2 - start2;
        rangeloop += duration2.count();
    }
    DEB("Average execution time: (int loop)   " << intloop/10000);
    DEB("Average execution time: (range loop) " << rangeloop/10000);*/
}
