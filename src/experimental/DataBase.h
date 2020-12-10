#ifndef __DB_H__
#define __DB_H__

#include <QPair>
#include <QMap>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include "src/database/dbinfo.h"
#include "debug.h"

#include "Entry.h"

namespace experimental {

// [F] ideas for functions of db class:
// https://github.com/fiffty-50/openpilotlog/wiki/New-DB-class-brainstorming

/*!
 * \brief The DB class encapsulates the SQL database by providing fast access
 * to hot database data.
 */


class DataBase {
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
    bool disconnect();

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
