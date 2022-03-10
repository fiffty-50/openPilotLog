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
#include "src/classes/aentry.h"
#include "src/classes/apilotentry.h"
#include "src/classes/atailentry.h"
#include "src/classes/aaircraftentry.h"
#include "src/classes/aflightentry.h"
#include "src/classes/astandardpaths.h"
#include "src/classes/acurrencyentry.h"
#include "src/classes/asimulatorentry.h"

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

    // Ensure DB is not copiable or assignable
    ADatabase(const ADatabase&) = delete;
    void operator=(const ADatabase&) = delete;
    static ADatabase* instance();

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
    bool exists(const AEntry &entry);
    bool exists(DataPosition data_position);

    /*!
     * \brief clear resets the database, i.e. deletes all content in the tables containing
     * userdata (pilots, flights, tails)
     */
    bool clear();

    /*!
     * \brief commits an entry to the database, calls either insert or update,
     * based on position data
     */
    bool commit(const AEntry &entry);

    /*!
     * \brief Create new entry in the databse based on UserInput
     */
    bool insert(const AEntry &new_entry);

    /*!
     * \brief Updates entry in database from existing entry tweaked by the user.
     */
    bool update(const AEntry &updated_entry);

    /*!
     * \brief deletes an entry from the database.
     */
    bool remove(const AEntry &entry);

    /*!
     * \brief deletes a list of entries from the database. Optimised for speed when
     * deleting many entries.
     */
    bool removeMany(QList<DataPosition>);

    /*!
     * \brief retreive entry data from the database to create an entry object
     */
    RowData_T getEntryData(DataPosition data_position);

    /*!
     * \brief retreive an Entry from the database.
     */
    AEntry getEntry(DataPosition data_position);

    /*!
     * \brief retreives a PilotEntry from the database.
     *
     * This function is a wrapper for DataBase::getEntry(DataPosition),
     * where the table is already set and which returns a PilotEntry
     * instead of an Entry. It allows for easy access to a pilot entry
     * with only the RowId required as input.
     */
    APilotEntry getPilotEntry(RowId_T row_id);

    /*!
     * \brief retreives a TailEntry from the database.
     *
     * This function is a wrapper for DataBase::getEntry(DataPosition),
     * where the table is already set and which returns a TailEntry
     * instead of an Entry. It allows for easy access to a tail entry
     * with only the RowId required as input.
     */
    ATailEntry getTailEntry(RowId_T row_id);

    /*!
     * \brief retreives a TailEntry from the database.
     *
     * This function is a wrapper for DataBase::getEntry(DataPosition),
     * where the table is already set and which returns an AAircraftEntry
     * instead of an AEntry. It allows for easy access to an aircraft entry
     * with only the RowId required as input.
     */
    AAircraftEntry getAircraftEntry(RowId_T row_id);

    /*!
     * \brief retreives a flight entry from the database.
     *
     * This function is a wrapper for DataBase::getEntry(DataPosition),
     * where the table is already set and which returns an AFlightEntry
     * instead of an AEntry. It allows for easy access to a flight entry
     * with only the RowId required as input.
     */
    AFlightEntry getFlightEntry(RowId_T row_id);

    /*!
     * \brief retreives a Simulator entry from the database.
     *
     * This function is a wrapper for DataBase::getEntry(DataPosition),
     * where the table is already set and which returns an ASimEntry
     * instead of an AEntry. It allows for easy access to a Simulator entry
     * with only the RowId required as input.
     */
    ASimulatorEntry getSimEntry(RowId_T row_id);

    /*!
     * \brief Retreives a currency entry from the database.
     */
    ACurrencyEntry getCurrencyEntry(ACurrencyEntry::CurrencyName currency_name);

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
     * \brief Resolves the foreign key in a flight entry
     * \return The Pilot Entry referencted by the foreign key.
     */
    APilotEntry resolveForeignPilot(RowId_T foreign_key);

    /*!
     * \brief Resolves the foreign key in a flight entry
     * \return The Tail Entry referencted by the foreign key.
     */
    ATailEntry resolveForeignTail(RowId_T foreign_key);

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
    QStringList getUserTableNames();

    /*!
     * \brief getTemplateTableNames returns a list of the table names of tables that contain template data
     * (aiports, aircraft,..)
     */
    QStringList getTemplateTableNames();

    /*!
     * \brief getUserDataState returns a struct containing the current amount of entries in the tails and
     * pilots tables.
     * \return
     */
    UserDataState getUserDataState();

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
