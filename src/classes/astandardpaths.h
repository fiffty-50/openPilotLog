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
#ifndef ASTANDARDPATHS_H
#define ASTANDARDPATHS_H

#include <QStandardPaths>
#include <QString>
#include <QMap>
#include <QDir>

/*!
 * \brief The AStandardAppPaths class encapsulates a static QMap holding
 * the standard paths of the application.
 */
class AStandardPaths{
public:
    enum Directories {
        Database,
        Templates,
        Backup,
        Log
    };
private:
    static QMap<Directories, QDir> directories;

    /*!
     * \brief Ensures the standard app directories exists and creates
     * them if neccessary.
     */
    static bool scan_directories();

public:
    /*!
     * \brief Creates and verifies a static map of the standard paths used in the app.
     */
    static bool setup();

    /*!
     * \brief Returns the QDir for the standard directory referenced
     * by the Directories enum 'loc'
     */
    static const QDir &directory(Directories location);

    /*!
     * \brief Returns a string of the absolute path to directory location concatenated with filename
     */
    static const QString asChildOfDir(Directories location, const QString& filename);
    /*!
     * \brief returns the static map of all standard directories
     * \return static const QMap<Directories, QDir>
     */
    static const QMap<Directories, QDir> &allDirectories();
};


#endif // ASTANDARDPATHS_H
