#ifndef __DB_H__
#define __DB_H__

#include <QPair>
#include <QMap>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include "src/database/dbinfo.h"
#include "debug.h"

#include "src/experimental/UserInput.h"

namespace experimental {

/// [F] ideas for functions of db class:
/// https://github.com/fiffty-50/openpilotlog/wiki/New-DB-class-brainstorming    

/// SELF DOCUMENTING CODE
using ColName = QString;
using ColData = QString;
using TableName = QString;
using RowId = int;
using DataPosition = QPair<TableName, RowId>;
using TableData = QMap<ColName, ColData>;
using ColumnData = QPair<ColName, ColData>;

// DEFAULTS
auto const DEFAULT_PILOT_POSITION = DataPosition("pilots", 0);

/*!
 * \brief The Entry class encapsulates table metadata(table name, row id)
 *  and data for new and existing entries in the database to operate on.
 */
class Entry {
public:
    DataPosition position;
protected:
    TableData tableData;
public:
    Entry() {};
    Entry(const Entry&) = default;
    Entry(DataPosition position_) : position(position_) {}
    void setData(TableData data) { tableData = data; }
    const TableData& getData() { return tableData; }
};

// [George]: Either with polymorphism or simple functions the result will be the same.
// if the following syntax is more clear to you we can switch to it.
// ... = new NewPilotDialog(Pilot(selectedPilots.first(), ...);
// the only difference will be that we will subclass Entry to have specialised
// constructor
class PilotEntry : public Entry {
public:
    PilotEntry() {};
    PilotEntry(const PilotEntry&) = default;
    PilotEntry(int row_id) : Entry::Entry(DataPosition("pilots", row_id)) {}
    PilotEntry(TableData fromNewPilotDialog) : Entry::Entry(DataPosition("pilots", 0)) {
        setData(fromNewPilotDialog);
    }
};

/*!
 * \brief The DB class encapsulates the SQL database by providing fast access
 * to hot database data.
 */
class DB {
private:
    static inline QStringList                   tableNames;
    static inline QMap<TableName, QStringList>  tableColumns;
public:
    /*!
     * \brief Connect to the database and populate database information.
     */
    static bool connect();

    /*!
     * \brief closes the database connection.
     */
    static bool disconnect();

    /*!
     * \brief Checks if an entry exists in the database, based on position data
     */
    static bool exists(Entry entry);

    /*!
     * \brief commits an entry to the database, calls either insert or update,
     * based on position data
     */
    static bool commit(Entry entry);

    /*!
     * \brief Create new entry in the databse based on UserInput
     */
    static bool insert(Entry newEntry);

    /*!
     * \brief Updates entry in database from existing entry tweaked by the user.
     */
    static bool update(Entry updated_entry);

    /*!
     * \brief deletes an entry from the database.
     */
    static bool remove(Entry entry);

};

}  // namespace experimental

#endif
