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
 */
#ifndef DATABASE_H
#define DATABASE_H

#include <QDir>
#include <QHash>
#include <QPair>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QString>

#include "src/classes/paths.h"
#include "src/database/entries/aircraftentry.h"
#include "src/database/entries/airportcodeentry.h"
#include "src/database/entries/airportentry.h"
#include "src/database/entries/flightdata.h"
#include "src/database/entries/flightsegmententry.h"
#include "src/database/entries/pilotentry.h"
#include "src/database/entries/row.h"
#include "src/database/entries/tailentry.h"
#include "src/gui/verification/flightdatabuilder.h"
#include "src/opl.h"

namespace OPL {

/*!
 * \brief Returns a pointer to the instance of the DataBase.
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
    Database() : m_databaseFile(Paths::databaseFileInfo()) {}
    const QFileInfo m_databaseFile;
    QStringList m_tableNames;
    QHash<QString, QStringList> m_tableColumns;
    QSqlError m_lastError;
    inline const static QString SQLITE_DRIVER = QStringLiteral("QSQLITE");

  public:
    Database(const Database &)       = delete;
    void operator=(const Database &) = delete;
    static Database *instance()
    {
        static Database instance;
        return &instance;
    }

    /*!
     * \brief Holds information about the last error that ocurred during
     * a SQL operation.
     * \details If the error type is QSqlError::UnknownError, the error is related to data
     * from the database (entry not found,...), otherwise the error is related to SQL execution. In
     * this case error.type() provides further information.
     *
     * If the error type is QSqlError::NoError, the last executed database query was successful.
     */
    inline QSqlError lastError() { return m_lastError; }

    /*!
     * \brief Holds information about the last error that ocurred during
     * a SQL operation.
     * \details This is a convenience function that returns the error text. To get the
     * full QSqlError, call lastError()
     */
    inline QString lastErrorText() { return m_lastError.text(); }

    /*!
     * \brief Connect to the database and populate database information.
     */
    bool connect();

    /*!
     * \brief closes the database connection.
     */
    void disconnect();

    /*!
     * \brief Updates the member variables tableNames and tableColumns with up-to-date layout
     * information if the database has been altered. This function is normally only required during
     * database setup or maintenance.
     */
    void updateLayout();

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
    const QStringList getTableColumns(DbTable table_name) const;

    /*!
     * \brief Can be used to access the database connection.
     * \return The QSqlDatabase object pertaining to the connection.
     */
    static inline QSqlDatabase database()
    {
        return QSqlDatabase::database(QStringLiteral("qt_sql_default_connection"));
    }

    /*!
     * \brief Checks if an entry exists in the database, based on position data
     */
    bool exists(const Row &row);

    /*!
     * \brief Checks if an entry exists in the database
     */
    bool exists(DbTable table, int row_id);

    /*!
     * \brief commits an entry to the database, calls either insert or update,
     * based on position data
     */
    bool commit(const Row &row);

    /*!
     * \brief commits data imported from JSON
     * \details This function is used to import values to the databases which are held in JSON
     * documents. These entries are pre-filled data used for providing completion data, such as
     * Airport or Aircraft Type Data.
     */
    bool commit(const QJsonArray &json_arr, const DbTable table);

    /*!
     * \brief commits Flight Data to the database.
     * \details Since a flight creates entries in separate tables, the FlightDataBuilder class
     * is used to collect and commit all the relevant data.
     */
    bool commit(FlightDataBuilder &flight_data);

    /*!
     * \brief deletes an entry from the database
     */
    bool remove(DbTable table, int row_id);

    /*!
     * \brief deletes an entry from the database
     */
    bool remove(const Row &row);

    /*!
     * \brief retreive a Map of <column name, column content> for a specific row in the database.
     */
    RowData_T getRowData(const DbTable table, const int row_id);

    /*!
     * \brief retreive a Map of <column name, column content> for a specific row in the database.
     * \details in this overload, a column other than the row_id can be specified. This enables
     * selection of entries "WHERE column = row_id", which is needed for retreiving content from
     * tables based on a foreign key.
     */
    RowData_T getRowData(const DbTable table, const QString &filterColumn, int row_id);

    /*!
     * \brief retreive a List of Maps of <column name, column content> representing rows in the
     * database.
     * \details This function can be used when more than one row of data is to be retreived from the
     * database. For a single row, use getRowData
     * */
    QList<RowData_T> getRowsData(const DbTable table, const QString &filterColumn, int row_id);

    /*!
     * \brief retreives a PilotEntry from the database. See row class for details.
     */
    inline PilotEntry getPilotEntry(int row_id)
    {
        const auto data = getRowData(DbTable::Pilots, row_id);
        return PilotEntry(row_id, data);
    }

    /*!
     * \brief get the database entry for the logbook owner (self)
     */
    inline PilotEntry getLogbookOwner()
    {
        auto data = getRowData(DbTable::Pilots, 1);
        data.insert(PilotEntry::ROWID, 1);
        return PilotEntry(1, data);
    }

    /*!
     * \brief retreives a TailEntry from the database. See row class for details.
     */
    inline TailEntry getTailEntry(int row_id)
    {
        const auto data = getRowData(DbTable::AircraftTails, row_id);
        return TailEntry(row_id, data);
    }

    /*!
     * \brief retreives a TailEntry from the database. See row class for details.
     */
    inline AircraftEntry getAircraftEntry(int row_id)
    {
        const auto data = getRowData(DbTable::AircraftTypes, row_id);
        return AircraftEntry(row_id, data);
    }

    FlightData getFlightData(int event_id);

    /*!
     * \brief Retreives an airport entry from the database. See row class for details.
     */
    inline AirportEntry getAirportEntry(int row_id)
    {
        const auto data = getRowData(DbTable::Airports, row_id);
        return AirportEntry(row_id, data);
    }

    /*!
     * \brief Retreives all airport code entries for a given airport_id
     * \param airport_id - the airport_id of an airport in the airports table
     * \return
     */
    QList<AirportCodeEntry> getAirportCodeEntries(int airport_id);

    QList<MovementEntry> getMovementEntries(int event_id);

    QList<FlightSegmentEntry> getFlightSegments(int flight_id);

    /*!
     * \brief returns the ROWID for the newest entry in the respective table.
     */
    int getLastEntry(DbTable table);

    /*!
     * \brief returns a list of ROWID's in the flights table for which foreign key constraints
     * exist.
     */
    QList<int> getForeignKeyConstraints(int foreign_row_id, DbTable table);

    /*!
     * \brief getTable returns all contents of a given table from the database
     * \return
     */
    QVector<RowData_T> getTable(DbTable table);

    /*!
     * \brief getUserTables returns a list of the of the tables that contain user-created data
     * (flights, pilots,..)
     */
    const QList<DbTable> &getUserTables() const;

    /*!
     * \brief getTemplateTables returns a list of the tables that contain template data
     * (aiports, aircraft,..)
     */
    const QList<DbTable> &getTemplateTables() const;

    /*!
     * \brief Database::createBackup copies the currently used database to an external backup
     * location provided by the user
     * \param dest_file This is the full path and filename of where the backup will be created, e.g.
     * 'home/Sully/myBackups/backupFromOpl.db'
     */
    bool createBackup(const QString &dest_file);

    /*!
     * \brief Database::restoreBackup restores the database from a given backup file and replaces
     * the currently active database.
     * \param backup_file This is the full path and filename of the backup, e.g.
     * 'home/Sully/myBackups/backupFromOpl.db'
     */
    bool restoreBackup(const QString &backup_file);

    void on_database_updated(DbTable table);

  private:
    /*!
     * \brief execute the query and emit databaseUpdated
     */
    bool exec(QSqlQuery &q, DbTable table = DbTable::Any);
    /*!
     * \brief execute the query but don't emit databaseUpdated
     */
    bool execQuietly(QSqlQuery &q);

    bool insert(const Row &new_row);

    bool insert(FlightDataBuilder &flight_data);

    bool update(const Row &updated_row);

    bool update(FlightDataBuilder &flight_data);

  signals:
    /*!
     * \brief updated is emitted whenever the database contents have been updated.
     * This can be either a commit, update or remove. This signal should be used to
     * trigger an update to the models of the views displaying database contents in
     * the user interface so that a user is always presented with up-to-date information.
     */
    void dataBaseUpdated(const DbTable table);
    /*!
     * \brief connectionReset is emitted whenever the database connection is reset, for
     * example when creating or restoring a backup.
     */
    void connectionReset();
};

} // namespace OPL

#endif // DATABASE_H
