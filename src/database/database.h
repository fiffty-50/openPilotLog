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
#ifndef DATABASE_H
#define DATABASE_H

#include <QPair>
#include <QHash>
#include <QString>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>

#include "src/classes/paths.h"
#include "src/opl.h"
#include "src/database/row.h"



namespace OPL {

/*!
 * \brief Convenience macro that returns instance of DataBase.
 * Instead of this:
 * OPL::DataBase::getInstance().commit(...)
 * Use this:
 * DB->commit(...)
 */
#define DB OPL::Database::instance()

/*!
 * \brief The DB class encapsulates the SQL database by providing fast access
 * to hot database data.
 */
class Database : public QObject {

private:
    Q_OBJECT
    Database()
        : databaseFile(OPL::Paths::databaseFileInfo())
    {}
    static Database* self;
    const QFileInfo databaseFile;
    QStringList tableNames;
    QHash<QString, QStringList> tableColumns;

    inline const static QString SQLITE_DRIVER  = QStringLiteral("QSQLITE");
    inline const static QList<OPL::DbTable> USER_TABLES = {
        OPL::DbTable::Flights,
        OPL::DbTable::Pilots,
        OPL::DbTable::Tails
    };
    inline const static QList<OPL::DbTable> TEMPLATE_TABLES = {
        OPL::DbTable::Aircraft,
        OPL::DbTable::Airports,
        OPL::DbTable::Currencies,
        OPL::DbTable::Changelog,
    };


public:
    Database(const Database&) = delete;
    void operator=(const Database&) = delete;
    static Database* instance();

    /*!
     * \brief Holds information about the last error that ocurred during
     * a SQL operation. If the error type is QSqlError::UnknownError, the error is related to data
     * from the database (entry not found,...), otherwise the error is related to SQL execution. In this
     * case error.type() provides further information.
     *
     * If the error type is QSqlError::NoError, the last executed database query was successful.
     */
    QSqlError lastError;

    /*!
     * \brief Connect to the database and populate database information.
     */
    bool connect();

    /*!
     * \brief closes the database connection.
     */
    void disconnect();

    /*!
     * \brief Updates the member variables tableNames and tableColumns with up-to-date layout information
     * if the database has been altered. This function is normally only required during database setup or maintenance.
     */
    void updateLayout();

    /*!
     * \brief Return the database revision number (not the sqlite version number).
     */
    const QString version() const;

    /*!
     * \brief Database::sqliteVersion returns the database sqlite version. See also dbRevision()
     * \return sqlite version string
     */
    const QString sqliteVersion() const;

    /*!
     * \brief Return the names of all tables in the database
     */
    const QStringList getTableNames() const;

    /*!
     * \brief Return the names of a given table in the database.
     */
    const QStringList getTableColumns(OPL::DbTable table_name) const;

    /*!
     * \brief Can be used to access the database connection.
     * \return The QSqlDatabase object pertaining to the connection.
     */
    static QSqlDatabase database();

    /*!
     * \brief Can be used to send a complex query to the database.
     * \param query - the full sql query statement
     * \param returnValues - the number of return values
     */
    QVector<QVariant> customQuery(QString statement, int return_values);

    /*!
     * \brief Checks if an entry exists in the database, based on position data
     */
    bool exists(const OPL::Row &row);

    /*!
     * \brief clear resets the database, i.e. deletes all content in the tables containing
     * userdata (pilots, flights, tails)
     */
    bool clear();

    /*!
     * \brief commits an entry to the database, calls either insert or update,
     * based on position data
     */
    bool commit(const OPL::Row &row);

    /*!
     * \brief commits data imported from JSON
     * \details This function is used to import values to the databases which are held in JSON documents.
     * These entries are pre-filled data used for providing completion data, such as Airport or Aircraft Type Data.
     */
    bool commit(const QJsonArray &json_arr, const OPL::DbTable table);

    /*!
     * \brief Create new entry in the databse based on UserInput
     */
    bool insert(const OPL::Row &new_row);

    /*!
     * \brief Updates entry in database from existing entry tweaked by the user.
     */
    bool update(const OPL::Row &updated_row);

    /*!
     * \brief deletes an entry from the database.
     */
    bool remove(const OPL::Row &row);

    /*!
     * \brief deletes a batch of entries from the database. Optimised for speed when
     * deleting many entries. The entries are identified using their row id
     */
    bool removeMany(OPL::DbTable table, const QList<int> &row_id_list);

    /*!
     * \brief retreive a Row from the database
     */
    OPL::Row getRow(const OPL::DbTable table, const int row_id);

    /*!
     * \brief retreive a Map of <column name, column content> for a specific row in the database.
     */
    RowData_T getRowData(const OPL::DbTable table, const int row_id);

    /*!
     * \brief retreives a PilotEntry from the database. See row class for details.
     */
    inline OPL::PilotEntry getPilotEntry(int row_id)
    {
        const auto data = getRowData(OPL::DbTable::Pilots, row_id);
        return OPL::PilotEntry(row_id, data);
    }

    /*!
     * \brief retreives a TailEntry from the database. See row class for details.
     */
    inline OPL::TailEntry getTailEntry(int row_id)
    {
        const auto data = getRowData(OPL::DbTable::Tails, row_id);
        return OPL::TailEntry(row_id, data);
    }

    /*!
     * \brief retreives a TailEntry from the database. See row class for details.
     */
    inline OPL::AircraftEntry getAircraftEntry(int row_id)
    {
        const auto data = getRowData(OPL::DbTable::Aircraft, row_id);
        return OPL::AircraftEntry(row_id, data);
    }

    /*!
     * \brief retreives a flight entry from the database. See row class for details.
     */
    inline OPL::FlightEntry getFlightEntry(int row_id)
    {
        const auto data = getRowData(OPL::DbTable::Flights, row_id);
        return OPL::FlightEntry(row_id, data);
    }

    /*!
     * \brief retreives a Simulator entry from the database. See row class for details.
     */
    inline OPL::SimulatorEntry getSimEntry(int row_id)
    {
        const auto data = getRowData(OPL::DbTable::Simulators, row_id);
        return OPL::SimulatorEntry(row_id, data);
    }

    /*!
     * \brief Retreives a currency entry from the database. See row class for details.
     */
    inline OPL::CurrencyEntry getCurrencyEntry(int row_id)
    {
        const auto data = getRowData(OPL::DbTable::Currencies, row_id);
        return OPL::CurrencyEntry(row_id, data);
    }

    /*!
     * \brief Retreives an airport entry from the database. See row class for details.
     */
    inline OPL::AirportEntry getAirportEntry(int row_id)
    {
        const auto data = getRowData(OPL::DbTable::Airports, row_id);
        return OPL::AirportEntry(row_id, data);
    }

    /*!
     * \brief returns the ROWID for the newest entry in the respective table.
     */
    int getLastEntry(OPL::DbTable table);

    /*!
     * \brief returns a list of ROWID's in the flights table for which foreign key constraints
     * exist.
     */
    QList<int> getForeignKeyConstraints(int foreign_row_id, OPL::DbTable table);

    /*!
     * \brief getTable returns all contents of a given table from the database
     * \return
     */
    QVector<RowData_T> getTable(OPL::DbTable table);

    /*!
     * \brief getUserTables returns a list of the of the tables that contain user-created data
     * (flights, pilots,..)
     */
    const QList<OPL::DbTable> &getUserTables() const;

    /*!
     * \brief getTemplateTables returns a list of the tables that contain template data
     * (aiports, aircraft,..)
     */
    const QList<OPL::DbTable> &getTemplateTables() const;

    // Maintenance and setup

    /*!
     * \brief Create or restore the database to its ready-to-use but empty state
     * \details The SQL code for the database creation is stored in a .sql file which is available as a ressource.
     * This file gets read, and the querys executed. If errors occur, returns false.
     */
    bool createSchema();
    /*!
     * \brief importTemplateData fills an empty database with the template
     * data (Aircraft, Airports, currencies, changelog) as read from the JSON
     * templates.
     * \param use_local_ressources determines whether the included ressource files
     * or a previously downloaded file should be used.
     * \return
     */
    bool importTemplateData(bool use_local_ressources);

    /*!
     * \brief Delete all rows from the user data tables (flights, pliots, tails)
     */
    bool resetUserData();

    /*!
     * \brief Database::createBackup copies the currently used database to an external backup location provided by the user
     * \param dest_file This is the full path and filename of where the backup will be created, e.g. 'home/Sully/myBackups/backupFromOpl.db'
     */
    bool createBackup(const QString& dest_file);

    /*!
     * \brief Database::restoreBackup restores the database from a given backup file and replaces the currently active database.
     * \param backup_file This is the full path and filename of the backup, e.g. 'home/Sully/myBackups/backupFromOpl.db'
     */
    bool restoreBackup(const QString& backup_file);



    /*!
     * @brief Retreive the total time of all flight entries in the databas
     * @param includePreviousExperience determines whether experience from previous logbooks
     * is included.
     * @return The sum of all entries in the flights table
     */
    const RowData_T getTotals(bool includePreviousExperience);
signals:
    /*!
     * \brief updated is emitted whenever the database contents have been updated.
     * This can be either a commit, update or remove. This signal should be used to
     * trigger an update to the models of the views displaying database contents in
     * the user interface so that a user is always presented with up-to-date information.
     */
    void dataBaseUpdated(const OPL::DbTable table);
    /*!
     * \brief connectionReset is emitted whenever the database connection is reset, for
     * example when creating or restoring a backup.
     */
    void connectionReset();
};

} // namespace OPL

#endif // DATABASE_H
