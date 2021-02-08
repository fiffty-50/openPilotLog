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
#include "src/classes/asettings.h"

// EASA FTL Limitations in minutes
// 100 hours per 28 days
static const int ROLLING_28_DAYS = 6000;
// 900 hours per calendar year
static const int CALENDAR_YEAR = 54000;
// 1000 hours per rolling 12 months
static const int ROLLING_12_MONTHS = 60000;

HomeWidget::HomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeWidget)
{
    ui->setupUi(this);
    ui->welcomeLabel->setText(tr("Welcome to openPilotLog, %1!").arg(userName()));


    displayLabels = {ui->TakeOffDisplayLabel, ui->LandingsDisplayLabel,
                     ui->FlightTime28dDisplayLabel, ui->FlightTimeCalYearDisplayLabel,
                     ui->FlightTime12mDisplayLabel};
    warningThreshold = ASettings::read(ASettings::UserData::FtlWarningThreshold).toDouble();
    DEB << "Filling Home Widget...";
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
    for (const auto &label : displayLabels)
        label->setStyleSheet(QString());

    fillTotals();
    fillCurrency();
    fillLimitations();
}

void HomeWidget::fillTotals()
{
    auto data = AStat::totals();
    for (const auto &field : data) {
        auto line_edit = this->findChild<QLineEdit *>(field.first + QLatin1String("LineEdit"));
        line_edit->setText(field.second);
    }
}

void HomeWidget::fillCurrency()
{
    auto takeoff_landings = AStat::countTakeOffLanding();

    ui->TakeOffDisplayLabel->setText(takeoff_landings[0].toString());
    if (takeoff_landings[0].toUInt() < 3)
        setLabelColour(ui->TakeOffDisplayLabel, HomeWidget::Red);
    ui->LandingsDisplayLabel->setText(takeoff_landings[1].toString());
    if (takeoff_landings[1].toUInt() < 3)
        setLabelColour(ui->LandingsDisplayLabel, HomeWidget::Red);

    QDate expiration_date = AStat::currencyTakeOffLandingExpiry();
    if (expiration_date == QDate::currentDate())
        setLabelColour(ui->currencyExpirationDisplayLabel, HomeWidget::Red);
    ui->currencyExpirationDisplayLabel->setText(expiration_date.toString(Qt::TextDate));
}

void HomeWidget::fillLimitations()
{
    int minutes = AStat::totalTime(AStat::TimeFrame::Rolling28Days);
    ui->FlightTime28dDisplayLabel->setText(ATime::toString(minutes));
    if (minutes >= ROLLING_28_DAYS) {
        setLabelColour(ui->FlightTime28dDisplayLabel, HomeWidget::Red);
    } else if (minutes >= ROLLING_28_DAYS * warningThreshold) {
        setLabelColour(ui->FlightTime28dDisplayLabel, HomeWidget::Orange);
    }

    minutes = AStat::totalTime(AStat::TimeFrame::Rolling12Months);
    ui->FlightTime12mDisplayLabel->setText(ATime::toString(minutes));
    if (minutes >= ROLLING_12_MONTHS) {
        setLabelColour(ui->FlightTime12mDisplayLabel, HomeWidget::Red);
    } else if (minutes >= ROLLING_12_MONTHS * warningThreshold) {
        setLabelColour(ui->FlightTime12mDisplayLabel, HomeWidget::Orange);
    }

    minutes = AStat::totalTime(AStat::TimeFrame::CalendarYear);
    ui->FlightTimeCalYearDisplayLabel->setText(ATime::toString(minutes));
    if (minutes >= CALENDAR_YEAR) {
        setLabelColour(ui->FlightTimeCalYearDisplayLabel, HomeWidget::Red);
    } else if (minutes >= CALENDAR_YEAR * warningThreshold) {
        setLabelColour(ui->FlightTimeCalYearDisplayLabel, HomeWidget::Orange);
    }
}

const QString HomeWidget::userName()
{
    auto statement = QStringLiteral("SELECT firstname FROM pilots WHERE ROWID=1");
    auto name = aDB->customQuery(statement, 1);
    if (!name.isEmpty())
        return name.first().toString();

    return QString();
}
