#ifndef __DB_H__
#define __DB_H__

#include <QPair>
#include <QMap>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include "src/database/dbinfo.h"
#include "debug.h"

#include "Entry.h"

namespace experimental {

/*!
 * \brief The DB class encapsulates the SQL database by providing fast access
 * to hot database data.
 */
class DataBase : public QObject {
    Q_OBJECT
private:
    TableNames tableNames;
    TableColumns tableColumns;
    static DataBase* instance;
    DataBase() = default;
public:
    // Ensure DB is not copiable or assignable
    DataBase(const DataBase&) = delete;
    void operator=(const DataBase&) = delete;
    static DataBase* getInstance();

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
     * \brief Checks if an entry exists in the database, based on position data
     */
    bool exists(Entry entry);

    /*!
     * \brief commits an entry to the database, calls either insert or update,
     * based on position data
     */
    bool commit(Entry entry);

    /*!
     * \brief Create new entry in the databse based on UserInput
     */
    bool insert(Entry newEntry);

    /*!
     * \brief Updates entry in database from existing entry tweaked by the user.
     */
    bool update(Entry updated_entry);

    /*!
     * \brief deletes an entry from the database.
     */
    bool remove(Entry entry);

    /*!
     * \brief retreive entry data from the database to create an entry object
     */
    TableData getEntryData(DataPosition);

    /*!
     * \brief retreive an Entry from the database.
     */
    Entry getEntry(DataPosition);

    /*!
     * \brief retreives a PilotEntry from the database.
     *
     * This function is a wrapper for DataBase::getEntry(DataPosition),
     * where the table is already set and which returns a PilotEntry
     * instead of an Entry. It allows for easy access to a pilot entry
     * with only the RowId required as input.
     */
    PilotEntry getPilotEntry(RowId);
    ///[F] the same can easily be implemented for tails/flights
signals:
    void commitSuccessful();

    void commitUnsuccessful(const QString &sqlError, const QString &sqlStatement);

};

/*!
 * \brief Convinience function that returns instance of DataBase.
 * Instead of this:
 * DataBase::getInstance().commit(...)
 * Write this:
 * DB()->commit(...)
 */
DataBase* DB();

}  // namespace experimental

#endif
