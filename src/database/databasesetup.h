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

/*! \brief Initial Database Setup
 * \details This class is responsible for setting up the database during the first run of the application.
 */
class DatabaseSetup
{
public:
    /*!
     * \brief Helper class to create a new database from scratch
     */
    DatabaseSetup() = default;

    /*! \brief Create the database tables
     * \return true if the setup was successful, false otherwise.
     */
    bool createTables();

    /*! \brief Create the database views
     * \return true if the setup was successful, false otherwise.
     */
    bool createViews();
    
    /*!
     * \brief Import template data into the database
     * \param useOnlineTemplateData If true, template data will be downloaded from the online repository
     * \details A set of template data for aircraft and airports is provided within the application assets.
     * However, this data may be outdated. If this flag is set to true, the latest template data will be downloaded
     * from the online repository during the database setup.
     * \return true if the import was successful, false otherwise.
     */
    bool importTemplateData(bool useOnlineTemplateData = false);
    
private:

    /*!
     * \brief Execute a SQL file
     * \param file_path The path to the SQL file to be executed.
     * \return true if the execution was successful, false otherwise.
     */
    bool executeSqlFile(const QString& file_path);

    bool importOnlineTemplateData();
    bool importLocalTemplateData();

    const QList<QString> m_tables = {
        QStringLiteral(":/database/schema/01_log_events.sql"),
        QStringLiteral(":/database/schema/02_flights.sql"),
        QStringLiteral(":/database/schema/03_simulators.sql"),
        QStringLiteral(":/database/schema/04_pilots.sql"),
        QStringLiteral(":/database/schema/05_aircraft_types.sql"),
        QStringLiteral(":/database/schema/06_aircraft_tails.sql"),
        QStringLiteral(":/database/schema/07_airports.sql"),
        QStringLiteral(":/database/schema/08_flight_segments.sql"),
        QStringLiteral(":/database/schema/09_movement_events.sql"),
        QStringLiteral(":/database/schema/10_approach_events.sql"),
        QStringLiteral(":/database/schema/11_airport_codes.sql"),
        QStringLiteral(":/database/schema/12_currencies.sql")
    };

    const QList<QString> m_views = {
        // TODO: add view sql files here
    };

    const QMap<OPL::DbTable, QString> m_templateData = {
        { OPL::DbTable::v2AircraftTypes, QStringLiteral(":/database/templates/aircraft_types.json") },
        { OPL::DbTable::v2Airports, QStringLiteral(":/database/templates/airports.json") },
        { OPL::DbTable::v2AirportCodes, QStringLiteral(":/database/templates/airport_codes.json")},
    };
};

#endif // DATABASESETUP_H
