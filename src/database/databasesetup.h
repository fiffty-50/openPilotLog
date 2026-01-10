/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
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
 *
 */

#ifndef DATABASESETUP_H
#define DATABASESETUP_H
#include <QtCore>
#include <QJsonArray>
#include "src/opl.h"
#include "src/database/database.h"
#include "src/classes/jsonhelper.h"

/*! \brief Initial Database Setup
 * \details This class is responsible for setting up the database during the first run of the application.
 */
class DatabaseSetup
{
public:
    /*!
    * \param useOnlineTemplateData If true, template data will be downloaded from the online repository
    * \details A set of template data for aircraft and airports is provided within the application assets.
    * However, this data may be outdated. If this flag is set to true, the latest template data will be downloaded
    * from the online repository during the database setup.
    */
    DatabaseSetup(bool useOnlineTemplateData)
        : m_useOnlineTemplateData(useOnlineTemplateData) {};

    /*! \brief Create the database tables
     * \return true if the setup was successful, false otherwise.
     */
    bool createTables();

    /*! \brief Create the database views
     * \return true if the setup was successful, false otherwise.
     */
    bool createViews();
    
    /*! \brief Import template data into the database
     * \return true if the import was successful, false otherwise.
     */
    bool importTemplateData();
    
private:

    /*! \brief Execute an SQL file
     * \param file_path The path to the SQL file to be executed.
     * \return true if the execution was successful, false otherwise.
     */
    bool executeSqlFile(const QString& file_path);

    bool importLocalTemplateData();
    bool importOnlineTemplateData();

    bool m_useOnlineTemplateData;

    QList<QString> m_tables = {
        QStringLiteral(":/database/.sql"),
        QStringLiteral(":/01_log_events.sql"),
        QStringLiteral(":/02_flights.sql"),
        QStringLiteral(":/03_simulators.sql"),
        QStringLiteral(":/04_pilots.sql"),
        QStringLiteral(":/05_aircraft_types.sql"),
        QStringLiteral(":/06_aircraft_tails.sql"),
        QStringLiteral(":/07_airports.sql"),
        QStringLiteral(":/08_flight_segments.sql"),
        QStringLiteral(":/09_movement_events.sql"),
        QStringLiteral(":/10_approach_events.sql"),
        QStringLiteral(":/11_airport_codes.sql"),
        QStringLiteral(":/12_currencies.sql")
    };

    QList<QString> m_views = {
        // TODO: add view sql files here
    };

    QHash<OPL::DbTable, QString> m_templateData = {
        { OPL::DbTable::Aircraft, QStringLiteral(":/database/templates/aircraft_types.json") },
        { OPL::DbTable::Airports, QStringLiteral(":/database/templates/airports.json") }
    };
};

#endif // DATABASESETUP_H
