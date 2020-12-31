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
    {Main::Theme, "theme"},
    {Main::ThemeID, "theme_id"}, // inconsistent naming
};

QMap<ASettings::UserData, QString> ASettings::userDataMap = {
    {UserData::LastName, "lastname"},
    {UserData::FirstName, "firstname" },
    {UserData::Company, "company"},
    {UserData::EmployeeID, "employeeid"},
    {UserData::Phone, "phone"},
    {UserData::Email, "email"},
    {UserData::DisplaySelfAs, "displayselfas"},
    {UserData::Alias, "alias"},
    {UserData::AcSortColumn, "acSortColumn"},  // [G]: inconsistent naming
    {UserData::PilSortColumn, "pilSortColumn"},
    {UserData::AcAllowIncomplete, "acAllowIncomplete"},
};

QMap<ASettings::FlightLogging, QString> ASettings::flightLoggingMap = {
    {FlightLogging::Function, "function"},
    {FlightLogging::Approach, "approach"},
    {FlightLogging::NightLogging, "nightlogging"},
    {FlightLogging::LogIFR, "logIfr"},
    {FlightLogging::FlightNumberPrefix, "flightnumberPrefix"},
    {FlightLogging::NumberTakeoffs, "numberTakeoffs"},
    {FlightLogging::NumberLandings, "numberLandings"},
    {FlightLogging::PopupCalendar,  "popupCalendar"},
    {FlightLogging::PilotFlying, "pilotFlying"},
    {FlightLogging::NightAngle, "nightangle"},
    {FlightLogging::Rules, "rules"},
};

QMap<ASettings::Setup, QString> ASettings::setupMap = {
    {Setup::SetupComplete, "setup_complete"},  // inconsistent naming
};

QMap<ASettings::NewFlight, QString> ASettings::newFlightMap = {
    {NewFlight::FunctionComboBox, "FunctionComboBox"},  // inconsistent naming
    {NewFlight::CalendarCheckBox, "calendarCheckBox"},
};

void ASettings::setup()
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings();
}

//
// Read/Write
//

QVariant ASettings::read(const Main key)
{ return QSettings().value(QStringLiteral("main/") + mainMap[key]); }

void ASettings::write(const Main key, const QVariant &val)
{ QSettings().setValue(QStringLiteral("main/") + mainMap[key], val); }

QVariant ASettings::read(const UserData key)
{ return QSettings().value(QStringLiteral("userdata/") + userDataMap[key]); }

void ASettings::write(const UserData key, const QVariant &val)
{ QSettings().setValue(QStringLiteral("userdata/") + userDataMap[key], val); }

QVariant ASettings::read(const FlightLogging key)
{ return QSettings().value(QStringLiteral("flightlogging/") + flightLoggingMap[key]); }

void ASettings::write(const FlightLogging key, const QVariant &val)
{ QSettings().setValue(QStringLiteral("flightlogging/") + flightLoggingMap[key], val); }

QVariant ASettings::read(const Setup key)
{ return QSettings().value(QStringLiteral("setup/") + setupMap[key]); }

void ASettings::write(const Setup key, const QVariant &val)
{ QSettings().setValue(QStringLiteral("setup/") + setupMap[key], val); }

QVariant ASettings::read(const NewFlight key)
{ return QSettings().value(QStringLiteral("NewFlight/") + newFlightMap[key]); }

void ASettings::write(const NewFlight key, const QVariant &val)
{ QSettings().setValue(QStringLiteral("NewFlight/") + newFlightMap[key], val); }

//
// to QString conversion
//
QString ASettings::stringOfKey(const Main key)
{ return mainMap[key]; }

QString ASettings::stringOfKey(const UserData key)
{ return userDataMap[key]; }

QString ASettings::stringOfKey(const FlightLogging key)
{ return flightLoggingMap[key]; }

QString ASettings::stringOfKey(const Setup key)
{ return setupMap[key]; }

QSettings ASettings::settings()
{ return QSettings(); }
