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
#include "settings.h"
#include <QSettings>


QMap<Settings::Main, QString> Settings::mainMap = {
    {Main::SetupComplete,               QStringLiteral("setupComplete")},
    {Main::Style,                       QStringLiteral("style")},
    {Main::Font,                        QStringLiteral("font")},
    {Main::FontSize,                    QStringLiteral("fontSize")},
    {Main::UseSystemFont,               QStringLiteral("useSystemFont")},
    {Main::LogbookView,                 QStringLiteral("logbookView")},
    {Main::DateFormat,                  QStringLiteral("dateFormat")},
};

QMap<Settings::UserData, QString> Settings::userDataMap = {
    {UserData::DisplaySelfAs,           QStringLiteral("displayselfas")},
    {UserData::TailSortColumn,          QStringLiteral("tailSortColumn")},
    {UserData::PilotSortColumn,         QStringLiteral("pilotSortColumn")},
    {UserData::FtlWarningThreshold,     QStringLiteral("ftlWarningThreshold")},
    {UserData::CurrWarningThreshold,    QStringLiteral("currWarningThreshold")},
    {UserData::ShowToLgdCurrency,       QStringLiteral("showToLdgCurrency")},
    {UserData::ShowLicCurrency,         QStringLiteral("showLicCurrency")},
    {UserData::ShowTrCurrency,          QStringLiteral("showTrCurrency")},
    {UserData::ShowLckCurrency,         QStringLiteral("showLckCurrency")},
    {UserData::ShowMedCurrency,         QStringLiteral("showMedCurrency")},
    {UserData::ShowCustom1Currency,     QStringLiteral("showCustom1Currency")},
    {UserData::ShowCustom2Currency,     QStringLiteral("showCustom2Currency")},
    {UserData::Custom1CurrencyName,     QStringLiteral("custom1CurrencyName")},
    {UserData::Custom2CurrencyName,     QStringLiteral("custom2CurrencyName")},
};

QMap<Settings::FlightLogging, QString> Settings::flightLoggingMap = {
    {FlightLogging::Function,           QStringLiteral("function")},
    {FlightLogging::Approach,           QStringLiteral("approach")},
    {FlightLogging::NightLoggingEnabled,QStringLiteral("nightLoggingEnabled")},
    {FlightLogging::LogIFR,             QStringLiteral("logIfr")},
    {FlightLogging::FlightNumberPrefix, QStringLiteral("flightnumberPrefix")},
    {FlightLogging::PilotFlying,        QStringLiteral("pilotFlying")},
    {FlightLogging::NightAngle,         QStringLiteral("nightangle")},
    //{FlightLogging::FlightTimeFormat,   QStringLiteral("flightTimeFormat")},
};

void Settings::setup()
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings();
}

/*!
 * \brief Settings::resetToDefaults (Re-)sets all settings to the default value
 */
void Settings::resetToDefaults()
{
    write(Main::Style, QStringLiteral("Fusion"));
    write(Main::UseSystemFont, true);
    write(Main::LogbookView, 0);
    write(Main::DateFormat, 0);

    write(UserData::DisplaySelfAs, 0);
    write(UserData::FtlWarningThreshold, 0.8); // To Do: UI Option
    write(UserData::CurrWarningThreshold, 30);
    write(UserData::ShowToLgdCurrency, true);
    write(UserData::ShowLicCurrency, false);
    write(UserData::ShowTrCurrency, false);
    write(UserData::ShowLckCurrency, false);
    write(UserData::ShowMedCurrency, false);
    write(UserData::ShowCustom1Currency, false);
    write(UserData::ShowCustom2Currency, false);

    write(FlightLogging::PilotFlying, true);
    write(FlightLogging::NightAngle, -6);
}

//
// Read/Write
//

QVariant Settings::read(const FlightLogging key)
{ return QSettings().value(groupOfKey(key)); }

void Settings::write(const FlightLogging key, const QVariant &val)
{ QSettings().setValue(groupOfKey(key), val); }

QVariant Settings::read(const Main key)
{ return QSettings().value(groupOfKey(key)); }

void Settings::write(const Main key, const QVariant &val)
{ QSettings().setValue(groupOfKey(key), val); }

QVariant Settings::read(const UserData key)
{ return QSettings().value(groupOfKey(key)); }

void Settings::write(const UserData key, const QVariant &val)
{ QSettings().setValue(groupOfKey(key), val); }

//
// QString conversion PATH
//
QString Settings::groupOfKey (const Settings::FlightLogging key)
{ return QStringLiteral("flightlogging/") + flightLoggingMap[key]; }

QString Settings::groupOfKey (const Settings::Main key)
{ return QStringLiteral("main/") + mainMap[key]; }

QString Settings::groupOfKey (const Settings::UserData key)
{ return QStringLiteral("userdata/") + userDataMap[key]; }

//
// QString conversion ONLY KEY
//
QString Settings::stringOfKey (const Settings::FlightLogging key)
{ return  flightLoggingMap[key]; }

QString Settings::stringOfKey (const Settings::Main key)
{ return  mainMap[key]; }

QString Settings::stringOfKey (const Settings::UserData key)
{ return  userDataMap[key]; }
