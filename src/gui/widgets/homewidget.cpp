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
    today = QDate::currentDate();
    currWarningThreshold = ASettings::read(ASettings::UserData::CurrWarningThreshold).toInt();
    ftlWarningThreshold = ASettings::read(ASettings::UserData::FtlWarningThreshold).toDouble();
    ui->welcomeLabel->setText(tr("Welcome to openPilotLog, %1!").arg(userName()));


    limitationDisplayLabels = {
        ui->TakeOffDisplayLabel,       ui->LandingsDisplayLabel,
        ui->FlightTime28dDisplayLabel, ui->FlightTimeCalYearDisplayLabel,
        ui->FlightTime12mDisplayLabel
    };

    DEB << "Filling Home Widget...";
    fillTotals();
    fillCurrencies();
    fillLimitations();
}

HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::onHomeWidget_dataBaseUpdated()
{
    for (const auto &label : limitationDisplayLabels)
        label->setStyleSheet(QString());

    fillTotals();
    fillCurrencyTakeOffLanding();
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

void HomeWidget::fillCurrency(ASettings::UserData date, QLabel* display_label)
{
    auto currency_date = ASettings::read(date).toDate();

    if (!currency_date.isValid())
        return;

    display_label->setText(currency_date.toString(Qt::TextDate));
    if (today.addDays(currWarningThreshold) >= currency_date) { // expires less than 30 days from today
        setLabelColour(display_label, Colour::Orange);
    }
    if (today >= currency_date) { // is expired
        setLabelColour(display_label, Colour::Red);
    }
}

void HomeWidget::fillCurrencies()
{
    fillCurrencyTakeOffLanding();

    ASettings::read(ASettings::UserData::ShowLicCurrency).toBool() ?
                fillCurrency(ASettings::UserData::ShowLicCurrency, ui->currLicDisplayLabel)
              : hideLabels(ui->currLicLabel, ui->currLicDisplayLabel);

    ASettings::read(ASettings::UserData::ShowTrCurrency).toBool() ?
                fillCurrency(ASettings::UserData::TrCurrencyDate, ui->currTrDisplayLabel)
              : hideLabels(ui->currTrLabel, ui->currTrDisplayLabel);

    ASettings::read(ASettings::UserData::ShowLckCurrency).toBool() ?
                fillCurrency(ASettings::UserData::LckCurrencyDate, ui->currLckDisplayLabel)
              : hideLabels(ui->currLckLabel, ui->currLckDisplayLabel);

    ASettings::read(ASettings::UserData::ShowMedCurrency).toBool() ?
                fillCurrency(ASettings::UserData::MedCurrencyDate, ui->currMedDisplayLabel)
              : hideLabels(ui->currMedLabel, ui->currMedDisplayLabel);

    ASettings::read(ASettings::UserData::ShowCustom1Currency).toBool() ?
                fillCurrency(ASettings::UserData::Custom1CurrencyDate, ui->currCustom1DisplayLabel)
              : hideLabels(ui->currCustom1Label, ui->currCustom1DisplayLabel);
    const QString custom1_text = ASettings::read(ASettings::UserData::Custom1CurrencyName).toString();
    if (!custom1_text.isEmpty())
        ui->currCustom1Label->setText(custom1_text);

    ASettings::read(ASettings::UserData::ShowCustom2Currency).toBool() ?
                fillCurrency(ASettings::UserData::Custom2CurrencyDate, ui->currCustom2DisplayLabel)
              : hideLabels(ui->currCustom2Label, ui->currCustom2DisplayLabel);
    const QString custom2_text = ASettings::read(ASettings::UserData::Custom2CurrencyName).toString();
    if (!custom2_text.isEmpty())
        ui->currCustom2Label->setText(custom2_text);
}

void HomeWidget::fillCurrencyTakeOffLanding()
{
    auto takeoff_landings = AStat::countTakeOffLanding();

    ui->TakeOffDisplayLabel->setText(takeoff_landings[0].toString());
    if (takeoff_landings[0].toUInt() < 3)
        setLabelColour(ui->TakeOffDisplayLabel, Colour::Red);
    ui->LandingsDisplayLabel->setText(takeoff_landings[1].toString());
    if (takeoff_landings[1].toUInt() < 3)
        setLabelColour(ui->LandingsDisplayLabel, Colour::Red);

    if (ASettings::read(ASettings::UserData::ShowToLgdCurrency).toBool()) {
        QDate expiration_date = AStat::currencyTakeOffLandingExpiry();
        if (expiration_date <= QDate::currentDate())
            setLabelColour(ui->currToLdgDisplayLabel, Colour::Red);
        ui->currToLdgDisplayLabel->setText(expiration_date.toString(Qt::TextDate));
    } else {
        ui->currToLdgLabel->hide();
        ui->currToLdgDisplayLabel->hide();
    }
}

void HomeWidget::fillLimitations()
{
    int minutes = AStat::totalTime(AStat::TimeFrame::Rolling28Days);
    ui->FlightTime28dDisplayLabel->setText(ATime::toString(minutes));
    if (minutes >= ROLLING_28_DAYS) {
        setLabelColour(ui->FlightTime28dDisplayLabel, Colour::Red);
    } else if (minutes >= ROLLING_28_DAYS * ftlWarningThreshold) {
        setLabelColour(ui->FlightTime28dDisplayLabel, Colour::Orange);
    }

    minutes = AStat::totalTime(AStat::TimeFrame::Rolling12Months);
    ui->FlightTime12mDisplayLabel->setText(ATime::toString(minutes));
    if (minutes >= ROLLING_12_MONTHS) {
        setLabelColour(ui->FlightTime12mDisplayLabel, Colour::Red);
    } else if (minutes >= ROLLING_12_MONTHS * ftlWarningThreshold) {
        setLabelColour(ui->FlightTime12mDisplayLabel, Colour::Orange);
    }

    minutes = AStat::totalTime(AStat::TimeFrame::CalendarYear);
    ui->FlightTimeCalYearDisplayLabel->setText(ATime::toString(minutes));
    if (minutes >= CALENDAR_YEAR) {
        setLabelColour(ui->FlightTimeCalYearDisplayLabel, Colour::Red);
    } else if (minutes >= CALENDAR_YEAR * ftlWarningThreshold) {
        setLabelColour(ui->FlightTimeCalYearDisplayLabel, Colour::Orange);
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
