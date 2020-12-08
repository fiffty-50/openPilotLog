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
    const DataPosition position;
protected:
    TableData table_data;
public:
    Entry() = delete;
    Entry(DataPosition position_) : position(position_) {}
    void populate_data(TableData data) { table_data = data; }
    const TableData& data() { return table_data; }
};

// [George]: Either with polymorphism or simple functions the result will be the same.
// if the following syntax is more clear to you we can switch to it.
// ... = new NewPilotDialog(Pilot(selectedPilots.first(), ...);
// the only difference will be that we will subclass Entry to have specialised
// constructor
class PilotEntry : public Entry {
    PilotEntry() = delete;
    PilotEntry(int row_id) : Entry::Entry(DataPosition("pilots", row_id)) {}
};

Entry newPilotEntry(int row_id) { return Entry(DataPosition("pilots", row_id)); }

static
bool insertPilot(UserInput& uin)
{
    DEB("Inserting...");
    auto data = uin.all();
    auto position = DEFAULT_PILOT_POSITION;

    if (data.isEmpty()) {
        DEB("Object Contains no data. Aborting.");
        return false;
    }
    QString statement = "INSERT INTO " + position.first + QLatin1String(" (");
    for (auto i = data.cbegin(); i != data.cend(); ++i) {
        statement += i.key() + QLatin1String(", ");
    }
    statement.chop(2);
    statement += QLatin1String(") VALUES (");
    for (auto i = data.cbegin(); i != data.cend(); ++i) {
        statement += QLatin1String("'") + i.value() + QLatin1String("', ");
    }
    statement.chop(2);
    statement += QLatin1String(")");

    QSqlQuery q(statement);
    if (q.lastError().type() == QSqlError::NoError) {
        DEB("Entry successfully committed.");
        return true;
    } else {
        DEB("Unable to commit. Query Error: " << q.lastError().text());
        return false;
    }
}

/*!
 * \brief The DB class encapsulates the SQL database by providing fast access
 * to hot database data.
 */
class DB {
private:
    static QStringList tableNames;
    static QMap<TableName, QStringList> tableColumns;
public:
    /*!
     * \brief Initialise DB class and populate columns.
     */
    static bool init();

    /*!
     * \brief Create new entry in the databse based on UserInput
     */
    static
    bool insert(UserInput uin)
    {
        switch (uin.meta_tag)
        {
        case UserInput::MetaTag::Pilot:
            DEB("Inserting NEW Pilot...");
            return insertPilot(uin);
        case UserInput::MetaTag::Flight:
        case UserInput::MetaTag::Aircraft:
        default:
            DEB("FALLTHROUGH in cases. Missing implementations");
            return false;
        }
    }

    static bool remove(UserInput uin) { return false; }

    static bool exists(UserInput uin) { return false; }

    /*!
     * \brief Updates entry in database from existing entry tweaked by the user.
     * New entry data is verified before commiting.
     */
    static bool update(Entry updated_entry)
    {
        auto position = updated_entry.position;
        auto data = updated_entry.data();
        QString statement = "UPDATE " + position.first + " SET ";
        for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
            if (i.key() != QString()) {
                statement += i.key() + QLatin1String("='") + i.value() + QLatin1String("', ");
            } else {
                DEB(i.key() << "is empty key. skipping.");
            }
        }
        statement.chop(2); // Remove last comma
        statement.append(QLatin1String(" WHERE _rowid_=") + QString::number(position.second));

        DEB("UPDATE QUERY: " << statement);
        QSqlQuery q(statement);
        //check result. Upon success, error should be " "
        if (q.lastError().type() == QSqlError::NoError)
        {
            DEB("Entry successfully committed.");
            return true;
        } else {
            DEB("Unable to commit. Query Error: " << q.lastError().text());
            return false;
        }
    }
};

}  // namespace experimental

#endif
