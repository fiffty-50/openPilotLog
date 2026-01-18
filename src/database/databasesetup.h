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
#include "src/opl.h"
#include <QJsonArray>
#include <QtCore>

/*! \brief Initial Database Setup
 * \details This class is responsible for setting up the database during the first run of the
 * application. It creates the necessary tables and views, and imports template data into the
 * database. It also provides functionality to reset the entire database or clear user data while
 * keeping the structure intact. These functions are destructive and will result in loss of data.
 * They should be used with caution and only after user confirmation as well as proper backups.
 */
class DatabaseSetup {
  public:
    /*!
     * \brief Helper class to create a new database from scratch
     */
    DatabaseSetup() = default;

    /*!
     * \brief Create the database tables
     * \return true if the setup was successful, false otherwise.
     */
    bool createTables();

    /*!
     * \brief Create the database views
     * \return true if the setup was successful, false otherwise.
     */
    bool createViews();

    /*!
     * \brief Import template data into the database
     * \param useOnlineTemplateData If true, template data will be downloaded from the online
     * repository
     * \details A set of template data for aircraft and airports is provided within the application
     * assets. However, this data may be outdated. If this flag is set to true, the latest template
     * data will be downloaded from the online repository during the database setup.
     * \return true if the import was successful, false otherwise.
     */
    bool importTemplateData(bool useOnlineTemplateData = false);

    /*!
     * \brief Clear the database by dropping all tables
     * \return true if the database was cleared successfully, false otherwise.
     */
    bool clearDatabase();

    /*!
     * \brief Clear all user data from the database tables, keeping the structure intact
     * \return true if the user data was cleared successfully, false otherwise.
     * \param useOnlineTemplateData If true, template data will be downloaded from the online
     * repository
     * \details This function clears all user data from the database tables, while keeping the table
     * structure intact. After clearing the user data, it imports the template data back into the
     * database.
     */
    bool clearUserData(bool useOnlineTemplateData = false);

  private:
    /*!
     * \brief Execute a SQL file
     * \param file_path The path to the SQL file to be executed.
     * \return true if the execution was successful, false otherwise.
     */
    bool executeSqlFile(const QString &file_path);

    bool importOnlineTemplateData();
    bool importLocalTemplateData();

    const QList<QString> m_tables = {
        QStringLiteral(":/db/schema/01_log_events.sql"),
        QStringLiteral(":/db/schema/02_flights.sql"),
        QStringLiteral(":/db/schema/03_simulators.sql"),
        QStringLiteral(":/db/schema/04_pilots.sql"),
        QStringLiteral(":/db/schema/05_aircraft_types.sql"),
        QStringLiteral(":/db/schema/06_aircraft_tails.sql"),
        QStringLiteral(":/db/schema/07_airports.sql"),
        QStringLiteral(":/db/schema/08_flight_segments.sql"),
        QStringLiteral(":/db/schema/09_movement_events.sql"),
        QStringLiteral(":/db/schema/10_approach_events.sql"),
        QStringLiteral(":/db/schema/11_airport_codes.sql"),
        QStringLiteral(":/db/schema/12_currencies.sql"),
        QStringLiteral(":/db/schema/99_indices.sql"),
    };

    const QList<QString> m_views = {
        QStringLiteral(":/db/views/01_AirportView.sql"),
        QStringLiteral(":/db/views/02_TailsView.sql"),
    };

    const QMap<OPL::DbTable, QString> m_templateData = {
        {OPL::DbTable::v2AircraftTypes, QStringLiteral(":/db/data/aircraft_types.json")},
        {OPL::DbTable::v2Airports,      QStringLiteral(":/db/data/airports.json")      },
        {OPL::DbTable::v2AirportCodes,  QStringLiteral(":/db/data/airport_codes.json") },
    };
};

#endif // DATABASESETUP_H
