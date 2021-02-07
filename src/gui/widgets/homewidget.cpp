/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#include "src/testing/adebug.h"
#include "src/database/adatabase.h"
#include "src/functions/atime.h"

HomeWidget::HomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeWidget)
{
    ui->setupUi(this);
    ui->welcomeLabel->setText(tr("Welcome to openPilotLog, %1!").arg(userName()));

    fillTotals();
    fillCurrency();
    fillLimitations();
}

HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::onHomeWidget_dataBaseUpdated()
{
    fillTotals();
    fillCurrency();
    fillLimitations();
}

void HomeWidget::fillTotals()
{
    auto data = AStat::totals();
    DEB << "Filling Totals Line Edits...";
    for (const auto &field : data) {
        auto line_edit = this->findChild<QLineEdit *>(field.first + QLatin1String("LineEdit"));
        line_edit->setText(field.second);
    }
}

void HomeWidget::fillCurrency()
{
    DEB << "Filling currency labels...";
    auto takeoff_landings = AStat::currencyTakeOffLanding();

    ui->TakeOffDisplayLabel->setText(takeoff_landings[0].toString());
    ui->LandingsDisplayLabel->setText(takeoff_landings[1].toString());
}

void HomeWidget::fillLimitations()
{
    DEB << "Filling limitations labels...";
    ui->FlightTime28dDisplayLabel->setText(ATime::toString(AStat::totalTime(AStat::Rolling28Days)));
    ui->FlightTime12mDisplayLabel->setText(ATime::toString(AStat::totalTime(AStat::RollingYear)));
    ui->FlightTimeCalYearDisplayLabel->setText(ATime::toString(AStat::totalTime(AStat::CalendarYear)));
}

const QString HomeWidget::userName()
{
    auto statement = QStringLiteral("SELECT firstname FROM pilots WHERE ROWID=1");
    auto name = aDB->customQuery(statement, 1);
    if (!name.isEmpty())
        return name.first().toString();

    return QString();
}
