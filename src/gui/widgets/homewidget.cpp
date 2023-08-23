/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
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
#include "src/gui/widgets/totalswidget.h"
#include "ui_homewidget.h"
#include "src/database/database.h"
#include "src/functions/time.h"
#include "src/classes/settings.h"
#include "src/database/row.h"

// EASA FTL Limitations in minutes
// 100 hours per 28 days
static const int ROLLING_28_DAYS = 6000;
// 900 hours per calendar year
static const int CALENDAR_YEAR = 54000;
// 1000 hours per rolling 12 months
static const int ROLLING_12_MONTHS = 60000;
// Todo: Encapsulate and plan to also use non-EASA (FAA,...) options

HomeWidget::HomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeWidget)
{
    ui->setupUi(this);
    today = QDate::currentDate();
    ftlWarningThreshold = Settings::read(Settings::UserData::FtlWarningThreshold).toDouble();
    currWarningThreshold = Settings::read(Settings::UserData::CurrWarningThreshold).toInt();
    auto logo = QPixmap(OPL::Assets::LOGO);
    ui->logoLabel->setPixmap(logo);
    ui->welcomeLabel->setText(tr("Welcome to openPilotLog, %1!").arg(userName()));


    limitationDisplayLabels = {
        ui->TakeOffDisplayLabel,       ui->LandingsDisplayLabel,
        ui->FlightTime28dDisplayLabel, ui->FlightTimeCalYearDisplayLabel,
        ui->FlightTime12mDisplayLabel
    };

    LOG << "Filling Home Widget...";
    fillTotals();
    fillSelectedCurrencies();
    fillLimitations();

    QObject::connect(DB,    &OPL::Database::dataBaseUpdated,
                     this,  &HomeWidget::onPilotsDatabaseChanged);
}

HomeWidget::~HomeWidget()
{
    delete ui;
}

void HomeWidget::refresh()
{
    LOG << "Updating HomeWidget...";
    const auto label_list = this->findChildren<QLabel *>();
    for (const auto label : label_list)
        label->setVisible(true);
    for (const auto &label : qAsConst(limitationDisplayLabels))
        label->setStyleSheet(QString());

    fillTotals();
    fillSelectedCurrencies();
    fillLimitations();
}

void HomeWidget::onPilotsDatabaseChanged(const OPL::DbTable table)
{
    // maybe logbook owner name has changed, redraw
    if (table == OPL::DbTable::Pilots)
        ui->welcomeLabel->setText(tr("Welcome to openPilotLog, %1!").arg(userName()));
}

void HomeWidget::changeEvent(QEvent *event)
{
    if (event != nullptr)
        if(event->type() == QEvent::LanguageChange)
            ui->retranslateUi(this);
}

/*!
 * \brief HomeWidget::fillTotals Retreives a Database Summary of Total Flight Time via the OPL::Statistics::totals
 * function and parses the return to fill out the QLineEdits.
 */
void HomeWidget::fillTotals()
{
    auto tw = new TotalsWidget(TotalsWidget::TotalTimeWidget, this);
    ui->totalsStackedWidget->addWidget(tw);
    ui->totalsStackedWidget->setCurrentWidget(tw);
}

void HomeWidget::fillCurrency(OPL::CurrencyName currency_name, QLabel* display_label)
{
    const auto currency_entry = DB->getCurrencyEntry(static_cast<int>(currency_name));

    if (currency_name == OPL::CurrencyName::Custom1) {
        ui->currCustom1Label->setText(currency_entry.getData().value(OPL::Db::CURRENCIES_CURRENCYNAME).toString());
    } else if (currency_name == OPL::CurrencyName::Custom2) {
        ui->currCustom2Label->setText(currency_entry.getData().value(OPL::Db::CURRENCIES_CURRENCYNAME).toString());
    }

    if (currency_entry.isValid()) {
        const auto currency_date = QDate::fromString(currency_entry.getData().value(
                                               OPL::Db::CURRENCIES_EXPIRYDATE).toString(),
                                               Qt::ISODate);
        display_label->setText(currency_date.toString(Qt::TextDate));
        setLabelColour(display_label, Colour::None);

        if (today >= currency_date) { // is expired
            setLabelColour(display_label, Colour::Red);
            return;
        } else if (today.addDays(currWarningThreshold) >=currency_date) { // expires less than <currWarningThreshold> days from current Date

            setLabelColour(display_label, Colour::Orange);
        }
    } else {
        display_label->setText(tr("Invalid Date"));
    }
}

/*!
 * \brief HomeWidget::fillSelectedCurrencies Checks whether a currency is selected and
 * retreives and displays relevant data.
 */
void HomeWidget::fillSelectedCurrencies()
{
    fillCurrencyTakeOffLanding();

    Settings::read(Settings::UserData::ShowLicCurrency).toBool() ?
                fillCurrency(OPL::CurrencyName::Licence, ui->currLicDisplayLabel)
              : hideLabels(ui->currLicLabel, ui->currLicDisplayLabel);
    Settings::read(Settings::UserData::ShowTrCurrency).toBool() ?
                fillCurrency(OPL::CurrencyName::TypeRating, ui->currTrDisplayLabel)
              : hideLabels(ui->currTrLabel, ui->currTrDisplayLabel);
    Settings::read(Settings::UserData::ShowLckCurrency).toBool() ?
                fillCurrency(OPL::CurrencyName::LineCheck, ui->currLckDisplayLabel)
              : hideLabels(ui->currLckLabel, ui->currLckDisplayLabel);
    Settings::read(Settings::UserData::ShowMedCurrency).toBool() ?
                fillCurrency(OPL::CurrencyName::Medical, ui->currMedDisplayLabel)
              : hideLabels(ui->currMedLabel, ui->currMedDisplayLabel);
    Settings::read(Settings::UserData::ShowCustom1Currency).toBool() ?
                fillCurrency(OPL::CurrencyName::Custom1, ui->currCustom1DisplayLabel)
              : hideLabels(ui->currCustom1Label, ui->currCustom1DisplayLabel);
    Settings::read(Settings::UserData::ShowCustom1Currency).toBool() ?
                fillCurrency(OPL::CurrencyName::Custom1, ui->currCustom1DisplayLabel)
              : hideLabels(ui->currCustom1Label, ui->currCustom1DisplayLabel);
    Settings::read(Settings::UserData::ShowCustom2Currency).toBool() ?
                fillCurrency(OPL::CurrencyName::Custom2, ui->currCustom2DisplayLabel)
              : hideLabels(ui->currCustom2Label, ui->currCustom2DisplayLabel);
}

/*!
 * \brief HomeWidget::fillCurrencyTakeOffLanding Uses OPL::Statistics::countTakeOffLandings to determine
 * the amount of Take-Offs and Landings in the last 90 days and displays data and notifications
 * as required.
 */
void HomeWidget::fillCurrencyTakeOffLanding()
{
    const auto takeoff_landings = OPL::Statistics::countTakeOffLanding();
    if(takeoff_landings.isEmpty())
        return;

    ui->TakeOffDisplayLabel->setText(takeoff_landings[0].toString());
    if (takeoff_landings[0].toUInt() < 3)
        setLabelColour(ui->TakeOffDisplayLabel, Colour::Red);
    ui->LandingsDisplayLabel->setText(takeoff_landings[1].toString());
    if (takeoff_landings[1].toUInt() < 3)
        setLabelColour(ui->LandingsDisplayLabel, Colour::Red);

    if (Settings::read(Settings::UserData::ShowToLgdCurrency).toBool()) {
        QDate expiration_date = OPL::Statistics::currencyTakeOffLandingExpiry();
        if (expiration_date <= QDate::currentDate())
            setLabelColour(ui->currToLdgDisplayLabel, Colour::Red);
        ui->currToLdgDisplayLabel->setText(expiration_date.toString(Qt::TextDate));
    } else {
        ui->currToLdgLabel->hide();
        ui->currToLdgDisplayLabel->hide();
    }
}

/*!
 * \brief HomeWidget::fillLimitations Queries OPL::Statistics to obtain information regarding cumulative
 * Flight Times and Calculates and Notifies about approaching Flight Time Limitations
 */
void HomeWidget::fillLimitations()
{
    int minutes = OPL::Statistics::totalTime(OPL::Statistics::TimeFrame::Rolling28Days);
    ui->FlightTime28dDisplayLabel->setText(OPL::Time::toString(minutes));
    if (minutes >= ROLLING_28_DAYS) {
        setLabelColour(ui->FlightTime28dDisplayLabel, Colour::Red);
    } else if (minutes >= ROLLING_28_DAYS * ftlWarningThreshold) {
        setLabelColour(ui->FlightTime28dDisplayLabel, Colour::Orange);
    }

    minutes = OPL::Statistics::totalTime(OPL::Statistics::TimeFrame::Rolling12Months);
    ui->FlightTime12mDisplayLabel->setText(OPL::Time::toString(minutes));
    if (minutes >= ROLLING_12_MONTHS) {
        setLabelColour(ui->FlightTime12mDisplayLabel, Colour::Red);
    } else if (minutes >= ROLLING_12_MONTHS * ftlWarningThreshold) {
        setLabelColour(ui->FlightTime12mDisplayLabel, Colour::Orange);
    }

    minutes = OPL::Statistics::totalTime(OPL::Statistics::TimeFrame::CalendarYear);
    ui->FlightTimeCalYearDisplayLabel->setText(OPL::Time::toString(minutes));
    if (minutes >= CALENDAR_YEAR) {
        setLabelColour(ui->FlightTimeCalYearDisplayLabel, Colour::Red);
    } else if (minutes >= CALENDAR_YEAR * ftlWarningThreshold) {
        setLabelColour(ui->FlightTimeCalYearDisplayLabel, Colour::Orange);
    }
}
