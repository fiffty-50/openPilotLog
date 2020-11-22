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


HomeWidget::HomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeWidget)
{
    ui->setupUi(this);
    showTotals();
}

HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::on_pushButton_clicked()
{
    NewFlightDialog nf(this, Flight(11), Db::editExisting);
    nf.exec();
}

void HomeWidget::showTotals()
{
    auto tw = new TotalsWidget(this);
    ui->stackedWidget->addWidget(tw);
    ui->stackedWidget->setCurrentWidget(tw);
    ui->stackedWidget->show();

}
