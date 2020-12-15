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
#ifndef __DB_H__
#define __DB_H__

#include <QPair>
#include <QMap>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include "src/database/dbinfo.h"
#include "src/testing/adebug.h"

#include "aentry.h"
#include "apilotentry.h"
#include "atailentry.h"
#include "aaircraftentry.h"
#include "aflightentry.h"

namespace experimental {

/*!
 * \brief The DB class encapsulates the SQL database by providing fast access
 * to hot database data.
 */
class ADataBase : public QObject {
    Q_OBJECT
private:
    TableNames tableNames;
    TableColumns tableColumns;
    static ADataBase* instance;
    ADataBase() = default;
public:
    // Ensure DB is not copiable or assignable
    ADataBase(const ADataBase&) = delete;
    void operator=(const ADataBase&) = delete;
    static ADataBase* getInstance();

    /*!
     * \brief The CompleterTarget enum provides the items for which QCompleter
     * completion lists are provided from the database.
     */
    enum CompleterTarget {airports, pilots, registrations, aircraft, companies};

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
    TableData getEntryData(DataPosition data_position);

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
     * \return
     */
    const QStringList getCompletionList(CompleterTarget);

    /*!
     * \brief getIdMap returns a map of a human-readable database value and
     * its row id. Used in the Dialogs to map user input to unique database entries.
     * \return
     */
    const QMap<QString, int> getIdMap(CompleterTarget);
signals:
    void sqlSuccessful();

    void sqlError(const QSqlError &sqlError, const QString &sqlStatement);

};

/*!
 * \brief Convinience function that returns instance of DataBase.
 * Instead of this:
 * DataBase::getInstance().commit(...)
 * Write this:
 * aDB()->commit(...)
 */
ADataBase* aDB();

}  // namespace experimental

#endif
