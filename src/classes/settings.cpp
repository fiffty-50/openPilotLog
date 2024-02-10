/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#include "src/classes/paths.h"

void Settings::init()
{
    LOG << "Initialising application settings...";
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, OPL::Paths::path(OPL::Paths::Settings));
    Settings::settingsInstance = new QSettings();
}

/*!
 * \brief Settings::resetToDefaults (Re-)sets all settings to the default value
 */
void Settings::resetToDefaults()
{
    setApplicationStyle(QStringLiteral("Fusion"));
    setUseSystemFont(true);
    setLogbookView(OPL::LogbookView::Default);
    setLogAsPilotFlying(true);
    setNightAngle(-6);
    setShowSelfAs(0);
    setFtlWarningThreshold(0.8);
    setCurrencyWarningThreshold(90);
    setPilotSortColumn(0);
    setTailSortColumn(0);
    setDisplayFormat(OPL::DateTimeFormat());

    sync();
}

OPL::DateTimeFormat Settings::getDisplayFormat()
{
    using namespace OPL;

    // date format
    const DateTimeFormat::DateFormat dateFormat = static_cast<DateTimeFormat::DateFormat>(
        settingsInstance->value(FORMAT_DATE_FORMAT, 0).toInt());
    const QString dateFormatString = settingsInstance->value(FORMAT_DATE_STRING, QStringLiteral("yyyy-MM-dd")).toString();
    // time format
    const DateTimeFormat::TimeFormat timeFormat = static_cast<DateTimeFormat::TimeFormat>(
        settingsInstance->value(FORMAT_TIME_FORMAT, 0).toInt());
    const QString timeFormatString = settingsInstance->value(FORMAT_TIME_STRING, QStringLiteral("hh:mm")).toString();

    return DateTimeFormat(dateFormat, dateFormatString, timeFormat, timeFormatString);

}

void Settings::setDisplayFormat(const OPL::DateTimeFormat &format)
{
    settingsInstance->setValue(FORMAT_DATE_FORMAT, static_cast<int>(format.dateFormat()));
    settingsInstance->setValue(FORMAT_DATE_STRING, format.dateFormatString());
    settingsInstance->setValue(FORMAT_TIME_FORMAT, static_cast<int>(format.timeFormat()));
    settingsInstance->setValue(FORMAT_TIME_STRING, format.timeFormatString());
}

