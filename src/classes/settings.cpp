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
    setDateFormat(OPL::DateFormat::ISODate);
    setLogAsPilotFlying(true);
    setNightAngle(-6);
    setShowSelfAs(0);
    setFtlWarningThreshold(0.8);
    setCurrencyWarningThreshold(90);
    setPilotSortColumn(0);
    setTailSortColumn(0);
    sync();
}

