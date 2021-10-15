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
#include "src/functions/alog.h"
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
// Todo: Encapsulate and plan to also use non-EASA (FAA,...) options

HomeWidget::HomeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomeWidget)
{
    ui->setupUi(this);
    today = QDate::currentDate();
    ftlWarningThreshold = ASettings::read(ASettings::UserData::FtlWarningThreshold).toDouble();
    auto logo = QPixmap(Opl::Assets::LOGO);
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

void HomeWidget::changeEvent(QEvent *event)
{
    if (event != nullptr)
        if(event->type() == QEvent::LanguageChange)
            ui->retranslateUi(this);
}

/*!
 * \brief HomeWidget::fillTotals Retreives a Database Summary of Total Flight Time via the AStat::totals
 * function and parses the return to fill out the QLineEdits.
 */
void HomeWidget::fillTotals()
{
    const auto data = AStat::totals();
    for (const auto &field : data) {
        auto line_edit = this->findChild<QLineEdit *>(field.first + QLatin1String("LineEdit"));
        line_edit->setText(field.second);
    }
}

void HomeWidget::fillCurrency(ACurrencyEntry::CurrencyName currency_name, QLabel* display_label)
{
    auto currency_entry = aDB->getCurrencyEntry(currency_name);
    if (currency_entry.isValid()) {
        auto currency_date = QDate::fromString(currency_entry.tableData.value(
                                                   Opl::Db::CURRENCIES_EXPIRYDATE).toString(),
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

    ASettings::read(ASettings::UserData::ShowLicCurrency).toBool() ?
                fillCurrency(ACurrencyEntry::CurrencyName::Licence, ui->currLicDisplayLabel)
              : hideLabels(ui->currLicLabel, ui->currLicDisplayLabel);
    ASettings::read(ASettings::UserData::ShowTrCurrency).toBool() ?
                fillCurrency(ACurrencyEntry::CurrencyName::TypeRating, ui->currTrDisplayLabel)
              : hideLabels(ui->currTrLabel, ui->currTrDisplayLabel);
    ASettings::read(ASettings::UserData::ShowLckCurrency).toBool() ?
                fillCurrency(ACurrencyEntry::CurrencyName::LineCheck, ui->currLckDisplayLabel)
              : hideLabels(ui->currLckLabel, ui->currLckDisplayLabel);
    ASettings::read(ASettings::UserData::ShowMedCurrency).toBool() ?
                fillCurrency(ACurrencyEntry::CurrencyName::Medical, ui->currMedDisplayLabel)
              : hideLabels(ui->currMedLabel, ui->currMedDisplayLabel);

    ASettings::read(ASettings::UserData::ShowCustom1Currency).toBool() ?
                fillCurrency(ACurrencyEntry::CurrencyName::Custom1, ui->currCustom1DisplayLabel)
              : hideLabels(ui->currCustom1Label, ui->currCustom1DisplayLabel);
    const QString custom1_text = ASettings::read(ASettings::UserData::Custom1CurrencyName).toString();
    if (!custom1_text.isEmpty())
        ui->currCustom1Label->setText(custom1_text);
    ASettings::read(ASettings::UserData::ShowCustom2Currency).toBool() ?
                fillCurrency(ACurrencyEntry::CurrencyName::Custom2, ui->currCustom2DisplayLabel)
              : hideLabels(ui->currCustom2Label, ui->currCustom2DisplayLabel);
    const QString custom2_text = ASettings::read(ASettings::UserData::Custom2CurrencyName).toString();
    if (!custom2_text.isEmpty())
        ui->currCustom2Label->setText(custom2_text);
}

/*!
 * \brief HomeWidget::fillCurrencyTakeOffLanding Uses AStat::countTakeOffLandings to determine
 * the amount of Take-Offs and Landings in the last 90 days and displays data and notifications
 * as required.
 */
void HomeWidget::fillCurrencyTakeOffLanding()
{
    const auto takeoff_landings = AStat::countTakeOffLanding();

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

/*!
 * \brief HomeWidget::fillLimitations Queries AStat to obtain information regarding cumulative
 * Flight Times and Calculates and Notifies about approaching Flight Time Limitations
 */
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
    const auto statement = QStringLiteral("SELECT firstname FROM pilots WHERE ROWID=1");
    const auto name = aDB->customQuery(statement, 1);
    if (!name.isEmpty())
        return name.first().toString();

    return QString();
}
