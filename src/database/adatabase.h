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
#ifndef ADATABASE_H
#define ADATABASE_H

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

#include "src/database/adatabasetypes.h"
#include "src/classes/row.h"

#include "src/classes/astandardpaths.h"

#define SQLITE_DRIVER QStringLiteral("QSQLITE")

/*!
 * \brief Convenience macro that returns instance of DataBase.
 * Instead of this:
 * DataBase::getInstance().commit(...)
 * Write this:
 * aDB->commit(...)
 */
#define aDB ADatabase::instance()

/*!
 * \brief The DBTarget enum lists database items that are
 * used by completers, for content matching or need to be accessed programatically.
 */
enum class ADatabaseTarget
{
    airport_identifier_icao,
    airport_identifier_iata,
    airport_identifier_all,
    airport_names,
    registrations,
    companies,
    tails,
    pilots,
    aircraft
};

/*!
 * \brief enumarates the tables in the database
 */
enum class ADatabaseTable
{
    // user tables
    tails = 0,
    pilots = 1,
    flights = 2,
    simulators = 3,
    currencies = 4,

    // template tables
    aircraft = 4,
    airports = 5,
    changelog = 7,
};

/*!
 * \brief Enumerates the QHash keys used when summarising a database
 */
enum class ADatabaseSummaryKey {
    total_flights,
    total_tails,
    total_pilots,
    last_flight,
    total_time,
};

/*!
 * \brief The UserDateState struct caches the current number of entries in relevant database tables
 * for fast access
 * \param numTails - Number of tails in the database
 * \param numPilots - Number of pilots in the database
 */
struct UserDataState {

    UserDataState(){numTails = 0; numPilots = 0;}
    UserDataState(int numTails_, int numPilots_)
        : numTails(numTails_), numPilots(numPilots_){}

    bool operator==(const UserDataState& other)
    {
        return numTails == other.numTails && numPilots == other.numPilots;
    }
    bool operator!=(const UserDataState& other)
    {
        return numTails != other.numTails || numPilots != other.numPilots;
    }

    int numTails;
    int numPilots;
};

/*!
 * \brief Custom Database Error derived from QSqlError.
 * Extends text() adding "Database Error: " before the text.
 * Errors that are related to SQL are assigned their respective error codes.
 * Errors that occur with data in the database are handled with the error code "opl"
 * and QSqlError::UnknownError
 */
class ADatabaseError : public QSqlError {
public:
    ADatabaseError() = default;
    ADatabaseError(QString msg);
    QString text() const;
    ADatabaseError(QSqlError);
};

/*!
 * \brief The DB class encapsulates the SQL database by providing fast access
 * to hot database data.
 */
class ADatabase : public QObject {

private:
    Q_OBJECT
    static ADatabase* self;
    TableNames_T tableNames;
    TableColumns_T tableColumns;
    int databaseRevision;

    ADatabase();
    int checkDbVersion() const;

    const static QStringList userTableNames;
    const static QStringList templateTableNames;
    const static int minimumDatabaseRevision;

public:
    ADatabase(const ADatabase&) = delete;
    void operator=(const ADatabase&) = delete;
    static ADatabase* instance();

    /*!
     * \brief Holds information about the last error that ocurred during
     * a SQL operation. If the error type is QSqlError::UnknownError, the error is related to data
     * from the database (entry not found,...), otherwise the error is related to SQL execution. In this
     * case error.type() provides further information.
     *
     * If the error type is QSqlError::NoError, the last executed database query was successful.
     */
    QSqlError lastError;

    const QFileInfo databaseFile;



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
    bool importTemplateData(bool use_local_ressources = true);

    /*!
     * \brief dbRevision returns the database Revision Number. The Revision refers to what iteration
     * of the database layout is used. For the sqlite version of the database refer to sqliteVersion()
     * \return
     */
    int dbRevision() const;

    /*!
     * \brief Return the names of all tables in the database
     */
    const TableNames_T getTableNames() const;

    /*!
     * \brief Return the names of a given table in the database.
     */
    const ColumnNames_T getTableColumns(TableName_T table_name) const;

    /*!
     * \brief Updates the member variables tableNames and tableColumns with up-to-date layout information
     * if the database has been altered. This function is normally only required during database setup or maintenance.
     */
    void updateLayout();

    /*!
     * \brief ADatabase::sqliteVersion returns the database sqlite version. See also dbRevision()
     * \return sqlite version string
     */
    const QString sqliteVersion() const;

    /*!
     * \brief Connect to the database and populate database information.
     */
    bool connect();

    /*!
     * \brief closes the database connection.
     */
    void disconnect();

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
    bool exists(const DataPosition &position);

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
    bool commit(const QJsonArray &json_arr, const QString &table_name);

    /*!
     * \brief Create new entry in the databse based on UserInput
     */
    //bool insert(const AEntry &new_entry);

    bool insert(const OPL::Row &new_row);

    /*!
     * \brief Updates entry in database from existing entry tweaked by the user.
     */
    //bool update(const AEntry &updated_entry);

    bool update(const OPL::Row &updated_row);

    /*!
     * \brief deletes an entry from the database.
     */
    //bool remove(const AEntry &entry);

    bool remove(const OPL::Row &row);

    /*!
     * \brief deletes a list of entries from the database. Optimised for speed when
     * deleting many entries.
     */
    bool removeMany(const QList<DataPosition> &position_list);

    /*!
     * \brief retreive entry data from the database to create an entry object
     */
    RowData_T getEntryData(const DataPosition &data_position);

    /*!
     * \brief retreive a Row from the database
     */
    OPL::Row getRow(const OPL::DbTable table, const int row_id);

    /*!
     * \brief retreive a Map of <column name, column content> for a specific row in the database.
     */
    RowData_T getRowData(const OPL::DbTable table, const int row_id);

    /*!
     * \brief retreives a PilotEntry from the database.
     *
     * This function is a wrapper for DataBase::getRowData,
     * where the table is already set and which returns a PilotEntry
     * instead of an Entry. It allows for easy access to a pilot entry
     * with only the RowId required as input.
     */
    inline OPL::PilotEntry getPilotEntry(RowId_T row_id)
    {
        const auto data = getRowData(OPL::DbTable::Pilots, row_id);
        return OPL::PilotEntry(row_id, data);
    }

    /*!
     * \brief retreives a TailEntry from the database.
     *
     * This function is a wrapper for DataBase::getRowData,
     * where the table is already set and which returns a TailEntry
     * instead of an Entry. It allows for easy access to a tail entry
     * with only the RowId required as input.
     */
    inline OPL::TailEntry getTailEntry(RowId_T row_id)
    {
        const auto data = getRowData(OPL::DbTable::Tails, row_id);
        return OPL::TailEntry(row_id, data);
    }

    /*!
     * \brief retreives a TailEntry from the database.
     *
     * This function is a wrapper for DataBase::getRowData,
     * where the table is already set and which returns an AAircraftEntry
     * instead of an AEntry. It allows for easy access to an aircraft entry
     * with only the RowId required as input.
     */
    inline OPL::AircraftEntry getAircraftEntry(RowId_T row_id)
    {
        const auto data = getRowData(OPL::DbTable::Aircraft, row_id);
        return OPL::AircraftEntry(row_id, data);
    }

    /*!
     * \brief retreives a flight entry from the database.
     *
     * This function is a wrapper for DataBase::getRowData,
     * where the table is already set and which returns an AFlightEntry
     * instead of an AEntry. It allows for easy access to a flight entry
     * with only the RowId required as input.
     */
    inline OPL::FlightEntry getFlightEntry(RowId_T row_id)
    {
        const auto data = getRowData(OPL::DbTable::Flights, row_id);
        return OPL::FlightEntry(row_id, data);
    }

    /*!
     * \brief retreives a Simulator entry from the database.
     *
     * This function is a wrapper for DataBase::getRowData,
     * where the table is already set and which returns an ASimEntry
     * instead of an AEntry. It allows for easy access to a Simulator entry
     * with only the RowId required as input.
     */
    inline OPL::SimulatorEntry getSimEntry(RowId_T row_id)
    {
        const auto data = getRowData(OPL::DbTable::Simulators, row_id);
        return OPL::SimulatorEntry(row_id, data);
    }

    /*!
     * \brief Retreives a currency entry from the database.
     */
    inline OPL::CurrencyEntry getCurrencyEntry(RowId_T row_id)
    {
        const auto data = getRowData(OPL::DbTable::Currencies, row_id);
        return OPL::CurrencyEntry(row_id, data);
    }

    /*!
     * \brief getCompletionList returns a QStringList of values for a
     * QCompleter based on database values
     */
    const QStringList getCompletionList(ADatabaseTarget target);

    /*!
     * \brief returns a QHash of a human-readable database value and
     * its row id. Used in the Dialogs to map user input to unique database entries.
     */
    const QHash<RowId_T, QString> getIdMap(ADatabaseTarget target);

    /*!
     * \brief returns the ROWID for the newest entry in the respective database.
     */
    RowId_T getLastEntry(ADatabaseTable table);

    /*!
     * \brief returns a list of ROWID's in the flights table for which foreign key constraints
     * exist.
     */
    QList<RowId_T> getForeignKeyConstraints(RowId_T foreign_row_id, ADatabaseTable target);

    /*!
     * \brief Return a summary of a database
     * \details Creates a summary of the database giving a quick overview of the relevant contents. The
     * function runs several specialised SQL queries to create a QHash<ADatabaseSummaryKey, QString> containing
     * Total Flight Time, Number of unique aircraft and pilots, as well as the date of last flight. Uses a temporary
     * database connection separate from the default connection in order to not tamper with the currently active
     * database connection.
     */
    QMap<ADatabaseSummaryKey, QString> databaseSummary(const QString& db_path);

    /*!
     * \brief returns a short summary string of the database, containing total time and date of last flight.
     */
    const QString databaseSummaryString(const QString& db_path);

    bool restoreBackup(const QString& backup_file);
    bool createBackup(const QString& dest_file);

    /*!
     * \brief getTable returns all contents of a given table from the database
     * \return
     */
    QVector<RowData_T> getTable(ADatabaseTable table_name);

    /*!
     * \brief getUserTableNames returns a list of the table names of tables that contain user-created data
     * (flights, pilots,..)
     */
    const QStringList &getUserTableNames() const;

    /*!
     * \brief getTemplateTableNames returns a list of the table names of tables that contain template data
     * (aiports, aircraft,..)
     */
    const QStringList &getTemplateTableNames() const;

    /*!
     * \brief getUserDataState returns a struct containing the current amount of entries in the tails and
     * pilots tables.
     * \return
     */
    UserDataState getUserDataState();

    /*!
     * \brief Delete all rows from the user data tables (flights, pliots, tails)
     */
    bool resetUserData();

    /*!
     * \brief getMinimumDatabaseRevision returns the minimum required database revision number required by the application.
     */
    static int getMinimumDatabaseRevision();

signals:
    /*!
     * \brief updated is emitted whenever the database contents have been updated.
     * This can be either a commit, update or remove. This signal should be used to
     * trigger an update to the models of the views displaying database contents in
     * the user interface so that a user is always presented with up-to-date information.
     */
    void dataBaseUpdated();
    /*!
     * \brief connectionReset is emitted whenever the database connection is reset, for
     * example when creating or restoring a backup.
     */
    void connectionReset();
};

#endif // ADATABASE_H
