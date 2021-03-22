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
#include "src/classes/astandardpaths.h"

QMap<AStandardPaths::Directories, QDir> AStandardPaths::directories;

bool AStandardPaths::setup()
{
    auto data_location = QStandardPaths::AppDataLocation;
    directories = {
        {Database, QDir(QStandardPaths::writableLocation(data_location))},
        {Templates, QDir(QStandardPaths::writableLocation(data_location)
         + QLatin1String("/templates"))},
        {Backup, QDir(QStandardPaths::writableLocation(data_location)
         + QLatin1String("/backup"))}
    };
    if (scan_directories())
        return true;

    return false;
}

const QDir& AStandardPaths::directory(Directories location)
{
    return directories[location];
}

const QString AStandardPaths::asChildOfDir(Directories location, const QString &filename)
{
    return directories[location].absoluteFilePath(filename);
}

const QMap<AStandardPaths::Directories, QDir>& AStandardPaths::allDirectories()
{
    return directories;
}

bool AStandardPaths::scan_directories()
{
    for(const auto& dir : directories){
        if(!dir.exists()) {
            DEB << dir << "Does not exist. Creating:" << dir.absolutePath();
            if (!dir.mkpath(dir.absolutePath()))
                return false;
        }
    }
    return true;
}
