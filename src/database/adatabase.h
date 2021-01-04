/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
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
#include <QMap>
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

#include "src/database/declarations.h"
#include "src/classes/aentry.h"
#include "src/classes/apilotentry.h"
#include "src/classes/atailentry.h"
#include "src/classes/aaircraftentry.h"
#include "src/classes/aflightentry.h"
#include "src/classes/astandardpaths.h"

#define DATABASE_VERSION 15

/*!
 * \brief Convinience macro that returns instance of DataBase.
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
    aircraft,
    airport_identifier_icao,
    airport_identifier_iata,
    airport_identifier_all,
    airport_names,
    pilots,
    registrations,
    companies,
    tails
};

// [G]: This is how we should handle custom "events" in the program.
// In this case a custom error doesnt need to be built from scratch.
// Find the type of error you want and extend it with a few tweaks.
/*!
 * \brief Custom Database Error derived from QSqlError.
 * Extends text() adding "Database Error: " before the text.
 */
class ADatabaseError : public QSqlError {
public:
  ADatabaseError() = default;
  ADatabaseError(QString msg);
  QString text() const;
};

/*!
 * \brief The DB class encapsulates the SQL database by providing fast access
 * to hot database data.
 */
class ADatabase : public QObject {
    Q_OBJECT
private:
    ADatabase();

    static ADatabase* self;
    TableNames tableNames;
    TableColumns tableColumns;
public:
    // Ensure DB is not copiable or assignable
    ADatabase(const ADatabase&) = delete;
    void operator=(const ADatabase&) = delete;
    static ADatabase* instance();
    TableNames getTableNames() const;
    ColumnNames getTableColumns(TableName table_name) const;
    void updateLayout();
    const QString sqliteVersion();

    ADatabaseError lastError;
    const QDir databaseDir;
    const QFileInfo databaseFile;

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
    QVector<QString> customQuery(QString statement, int return_values);

    /*!
     * \brief Checks if an entry exists in the database, based on position data
     */
    bool exists(AEntry entry);
    bool exists(DataPosition data_position);

    /*!
     * \brief commits an entry to the database, calls either insert or update,
     * based on position data
     */
    bool commit(AEntry entry);

    /*!
     * \brief Create new entry in the databse based on UserInput
     */
    bool insert(AEntry new_entry);

    /*!
     * \brief Updates entry in database from existing entry tweaked by the user.
     */
    bool update(AEntry updated_entry);

    /*!
     * \brief deletes an entry from the database.
     */
    bool remove(AEntry entry);

    /*!
     * \brief deletes a list of entries from the database. Optimised for speed when
     * deleting many entries.
     */
    bool removeMany(QList<DataPosition>);

    /*!
     * \brief retreive entry data from the database to create an entry object
     */
    RowData getEntryData(DataPosition data_position);

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
    APilotEntry getPilotEntry(RowId row_id);

    /*!
     * \brief retreives a TailEntry from the database.
     *
     * This function is a wrapper for DataBase::getEntry(DataPosition),
     * where the table is already set and which returns a TailEntry
     * instead of an Entry. It allows for easy access to a tail entry
     * with only the RowId required as input.
     */
    ATailEntry getTailEntry(RowId row_id);

    /*!
     * \brief retreives a TailEntry from the database.
     *
     * This function is a wrapper for DataBase::getEntry(DataPosition),
     * where the table is already set and which returns an AAircraftEntry
     * instead of an AEntry. It allows for easy access to an aircraft entry
     * with only the RowId required as input.
     */
    AAircraftEntry getAircraftEntry(RowId row_id);

    /*!
     * \brief retreives a flight entry from the database.
     *
     * This function is a wrapper for DataBase::getEntry(DataPosition),
     * where the table is already set and which returns an AFlightEntry
     * instead of an AEntry. It allows for easy access to a flight entry
     * with only the RowId required as input.
     */
    AFlightEntry getFlightEntry(RowId row_id);

    /*!
     * \brief getCompletionList returns a QStringList of values for a
     * QCompleter based on database values
     */
    const QStringList getCompletionList(ADatabaseTarget);

    /*!
     * \brief returns a QMap<QString, int> of a human-readable database value and
     * its row id. Used in the Dialogs to map user input to unique database entries.
     */
    const QMap<QString, int> getIdMap(ADatabaseTarget);

    /*!
     * \brief returns the ROWID for the newest entry in the respective database.
     */
    int getLastEntry(ADatabaseTarget);

    /*!
     * \brief returns a list of ROWID's in the flights table for which foreign key constraints
     * exist.
     */
    QList<int> getForeignKeyConstraints(int foreign_row_id, ADatabaseTarget target);

    /*!
     * \brief Resolves the foreign key in a flight entry
     * \return The Pilot Entry referencted by the foreign key.
     */
    APilotEntry resolveForeignPilot(int foreign_key);

    /*!
     * \brief Resolves the foreign key in a flight entry
     * \return The Tail Entry referencted by the foreign key.
     */
    ATailEntry resolveForeignTail(int foreign_key);





signals:
    /*!
     * \brief updated is emitted whenever the database contents have been updated.
     * This can be either a commit, update or remove. This signal should be used to
     * trigger an update to the models of the views displaying database contents in
     * the user interface so that a user is always presented with up-to-date information.
     */
    void dataBaseUpdated();
};

#endif // ADATABASE_H
