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
#include "asettings.h"
#include <QSettings>


QMap<ASettings::Main, QString> ASettings::mainMap = {
    {Main::SetupComplete,               QStringLiteral("setupComplete")},
    {Main::Style,                       QStringLiteral("style")},
    {Main::Font,                        QStringLiteral("font")},
    {Main::FontSize,                    QStringLiteral("fontSize")},
    {Main::UseSystemFont,               QStringLiteral("useSystemFont")},
    {Main::LogbookView,                 QStringLiteral("logbookView")},
};

QMap<ASettings::UserData, QString> ASettings::userDataMap = {
    {UserData::DisplaySelfAs,           QStringLiteral("displayselfas")},
    {UserData::TailSortColumn,          QStringLiteral("tailSortColumn")},
    {UserData::PilotSortColumn,         QStringLiteral("pilotSortColumn")},
    {UserData::AcftAllowIncomplete,     QStringLiteral("acftAllowIncomplete")},
    {UserData::FtlWarningThreshold,     QStringLiteral("ftlWarningThreshold")},
    {UserData::CurrWarningEnabled,      QStringLiteral("currWarningEnabled")},
    {UserData::CurrWarningThreshold,    QStringLiteral("currWarningThreshold")},
    {UserData::ShowToLgdCurrency,       QStringLiteral("showToLdgCurrency")},
    {UserData::ShowLicCurrency,         QStringLiteral("showLicCurrency")},
    {UserData::ShowTrCurrency,          QStringLiteral("showTrCurrency")},
    {UserData::ShowLckCurrency,         QStringLiteral("showLckCurrency")},
    {UserData::ShowMedCurrency,         QStringLiteral("showMedCurrency")},
    {UserData::ShowCustom1Currency,     QStringLiteral("showCustom1Currency")},
    {UserData::ShowCustom2Currency,     QStringLiteral("showCustom2Currency")},
    {UserData::LicCurrencyDate,         QStringLiteral("licCurrencyDate")},
    {UserData::TrCurrencyDate,          QStringLiteral("trCurrencyDate")},
    {UserData::LckCurrencyDate,         QStringLiteral("lckCurrencyDate")},
    {UserData::MedCurrencyDate,         QStringLiteral("medCurrencyDate")},
    {UserData::Custom1CurrencyDate,     QStringLiteral("custom1CurrencyDate")},
    {UserData::Custom2CurrencyDate,     QStringLiteral("custom2CurrencyDate")},
    {UserData::Custom1CurrencyName,     QStringLiteral("custom1CurrencyName")},
    {UserData::Custom2CurrencyName,     QStringLiteral("custom2CurrencyName")},
};

QMap<ASettings::FlightLogging, QString> ASettings::flightLoggingMap = {
    {FlightLogging::Function,           QStringLiteral("function")},
    {FlightLogging::Approach,           QStringLiteral("approach")},
    {FlightLogging::NightLoggingEnabled,QStringLiteral("nightLoggingEnabled")},
    {FlightLogging::LogIFR,             QStringLiteral("logIfr")},
    {FlightLogging::FlightNumberPrefix, QStringLiteral("flightnumberPrefix")},
    {FlightLogging::NumberTakeoffs,     QStringLiteral("numberTakeoffs")},
    {FlightLogging::NumberLandings,     QStringLiteral("numberLandings")},
    {FlightLogging::PopupCalendar,      QStringLiteral("popupCalendar")},
    {FlightLogging::PilotFlying,        QStringLiteral("pilotFlying")},
    {FlightLogging::NightAngle,         QStringLiteral("nightangle")},
    {FlightLogging::Rules,              QStringLiteral("rules")},
    {FlightLogging::FlightTimeFormat,   QStringLiteral("flightTimeFormat")},
    {FlightLogging::FunctionComboBox,   QStringLiteral("functionComboBox")},
    {FlightLogging::CalendarCheckBox,   QStringLiteral("calendarCheckBox")},
};

void ASettings::setup()
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings();
}

/*!
 * \brief ASettings::resetToDefaults (Re-)sets all settings to the default value
 */
void ASettings::resetToDefaults()
{
    write(Main::Style, QStringLiteral("Fusion"));
    write(Main::UseSystemFont, true);
    write(Main::LogbookView, 0);

    write(UserData::DisplaySelfAs, 0);
    write(UserData::FtlWarningThreshold, 0.8); // To Do: UI Option
    write(UserData::CurrWarningEnabled, true);
    write(UserData::CurrWarningThreshold, 30);
    write(UserData::ShowToLgdCurrency, true);
    write(UserData::ShowLicCurrency, false);
    write(UserData::ShowTrCurrency, false);
    write(UserData::ShowLckCurrency, false);
    write(UserData::ShowMedCurrency, false);
    write(UserData::ShowCustom1Currency, false);
    write(UserData::ShowCustom2Currency, false);

    write(FlightLogging::NumberTakeoffs, 1);
    write(FlightLogging::NumberLandings, 1);
    write(FlightLogging::PopupCalendar, true);
    write(FlightLogging::PilotFlying, true);
    write(FlightLogging::NightAngle, -6);
}

//
// Read/Write
//

QVariant ASettings::read(const FlightLogging key)
{ return QSettings().value(groupOfKey(key)); }

void ASettings::write(const FlightLogging key, const QVariant &val)
{ QSettings().setValue(groupOfKey(key), val); }

QVariant ASettings::read(const Main key)
{ return QSettings().value(groupOfKey(key)); }

void ASettings::write(const Main key, const QVariant &val)
{ QSettings().setValue(groupOfKey(key), val); }

QVariant ASettings::read(const UserData key)
{ return QSettings().value(groupOfKey(key)); }

void ASettings::write(const UserData key, const QVariant &val)
{ QSettings().setValue(groupOfKey(key), val); }

//
// QString conversion PATH
//
QString ASettings::groupOfKey (const ASettings::FlightLogging key)
{ return QStringLiteral("flightlogging/") + flightLoggingMap[key]; }

QString ASettings::groupOfKey (const ASettings::Main key)
{ return QStringLiteral("main/") + mainMap[key]; }

QString ASettings::groupOfKey (const ASettings::UserData key)
{ return QStringLiteral("userdata/") + userDataMap[key]; }

//
// QString conversion ONLY KEY
//
QString ASettings::stringOfKey (const ASettings::FlightLogging key)
{ return  flightLoggingMap[key]; }

QString ASettings::stringOfKey (const ASettings::Main key)
{ return  mainMap[key]; }

QString ASettings::stringOfKey (const ASettings::UserData key)
{ return  userDataMap[key]; }
