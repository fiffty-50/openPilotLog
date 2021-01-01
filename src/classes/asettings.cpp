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
#include "asettings.h"
#include "src/astandardpaths.h"
#include <QSettings>


QMap<ASettings::Main, QString> ASettings::mainMap = {
    {Main::Theme,   QStringLiteral("theme")},
    {Main::ThemeID, QStringLiteral("theme_id")}, // inconsistent naming
};

QMap<ASettings::LogBook, QString> ASettings::logBookMap = {
    {LogBook::View, QStringLiteral("view")},
};

QMap<ASettings::UserData, QString> ASettings::userDataMap = {
    {UserData::LastName,          QStringLiteral("lastname")},
    {UserData::FirstName,         QStringLiteral("firstname") },
    {UserData::Company,           QStringLiteral("company")},
    {UserData::EmployeeID,        QStringLiteral("employeeid")},
    {UserData::Phone,             QStringLiteral("phone")},
    {UserData::Email,             QStringLiteral("email")},
    {UserData::DisplaySelfAs,     QStringLiteral("displayselfas")},
    {UserData::Alias,             QStringLiteral("alias")},
    {UserData::AcSortColumn,      QStringLiteral("acSortColumn")},  // [G]: inconsistent naming
    {UserData::PilSortColumn,     QStringLiteral("pilSortColumn")},
    {UserData::AcAllowIncomplete, QStringLiteral("acAllowIncomplete")},
};

QMap<ASettings::FlightLogging, QString> ASettings::flightLoggingMap = {
    {FlightLogging::Function,           QStringLiteral("function")},
    {FlightLogging::Approach,           QStringLiteral("approach")},
    {FlightLogging::NightLogging,       QStringLiteral("nightlogging")},
    {FlightLogging::LogIFR,             QStringLiteral("logIfr")},
    {FlightLogging::FlightNumberPrefix, QStringLiteral("flightnumberPrefix")},
    {FlightLogging::NumberTakeoffs,     QStringLiteral("numberTakeoffs")},
    {FlightLogging::NumberLandings,     QStringLiteral("numberLandings")},
    {FlightLogging::PopupCalendar,      QStringLiteral("popupCalendar")},
    {FlightLogging::PilotFlying,        QStringLiteral("pilotFlying")},
    {FlightLogging::NightAngle,         QStringLiteral("nightangle")},
    {FlightLogging::Rules,              QStringLiteral("rules")},
};

QMap<ASettings::Setup, QString> ASettings::setupMap = {
    {Setup::SetupComplete, QStringLiteral("setup_complete")},  // inconsistent naming
};

QMap<ASettings::NewFlight, QString> ASettings::newFlightMap = {
    {NewFlight::FunctionComboBox, QStringLiteral("FunctionComboBox")},  // inconsistent naming
    {NewFlight::CalendarCheckBox, QStringLiteral("calendarCheckBox")},
};

void ASettings::setup()
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings();
}

//
// Read/Write
//

QVariant ASettings::read(const FlightLogging key)
{ return QSettings().value(pathOfKey(key)); }

void ASettings::write(const FlightLogging key, const QVariant &val)
{ QSettings().setValue(pathOfKey(key), val); }

QVariant ASettings::read(const LogBook key)
{ return QSettings().value(pathOfKey(key)); }

void ASettings::write(const LogBook key, const QVariant &val)
{ QSettings().setValue(pathOfKey(key), val); }

QVariant ASettings::read(const Main key)
{ return QSettings().value(pathOfKey(key)); }

void ASettings::write(const Main key, const QVariant &val)
{ QSettings().setValue(pathOfKey(key), val); }

QVariant ASettings::read(const Setup key)
{ return QSettings().value(pathOfKey(key)); }

void ASettings::write(const Setup key, const QVariant &val)
{ QSettings().setValue(pathOfKey(key), val); }

QVariant ASettings::read(const NewFlight key)
{ return QSettings().value(pathOfKey(key)); }

void ASettings::write(const NewFlight key, const QVariant &val)
{ QSettings().setValue(pathOfKey(key), val); }

QVariant ASettings::read(const UserData key)
{ return QSettings().value(pathOfKey(key)); }

void ASettings::write(const UserData key, const QVariant &val)
{ QSettings().setValue(pathOfKey(key), val); }

//
// QString conversion PATH
//
QString ASettings::pathOfKey (const ASettings::FlightLogging key)
{ return QStringLiteral("flightlogging/") + flightLoggingMap[key]; }

QString ASettings::pathOfKey (const ASettings::LogBook key)
{ return QStringLiteral("logbook/") + logBookMap[key]; }

QString ASettings::pathOfKey (const ASettings::Main key)
{ return QStringLiteral("main/") + mainMap[key]; }

QString ASettings::pathOfKey (const ASettings::NewFlight key)
{ return QStringLiteral("NewFlight/") + newFlightMap[key]; }

QString ASettings::pathOfKey (const ASettings::Setup key)
{ return QStringLiteral("setup/") + setupMap[key]; }

QString ASettings::pathOfKey (const ASettings::UserData key)
{ return QStringLiteral("userdata/") + userDataMap[key]; }

//
// QString conversion ONLY KEY
//
QString ASettings::stringOfKey (const ASettings::FlightLogging key)
{ return  flightLoggingMap[key]; }

QString ASettings::stringOfKey (const ASettings::LogBook key)
{ return  logBookMap[key]; }

QString ASettings::stringOfKey (const ASettings::Main key)
{ return  mainMap[key]; }

QString ASettings::stringOfKey (const ASettings::NewFlight key)
{ return  newFlightMap[key]; }

QString ASettings::stringOfKey (const ASettings::Setup key)
{ return  setupMap[key]; }

QString ASettings::stringOfKey (const ASettings::UserData key)
{ return  userDataMap[key]; }



QSettings ASettings::settings()
{ return QSettings(); }
