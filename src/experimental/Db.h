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

using ColName = QString;
using ColData = QString;
using DataPosition = QPair<QString, int>;
using TableData = QMap<ColName, ColData>;

class Entry {
public:
    const DataPosition position;
private:
    TableData table_data;
public:
    Entry() = delete;
    Entry(DataPosition position_)
        : position(position_) {}
    void populate_data(TableData data) { table_data = data; }
    const TableData& data() { return table_data; }
};


Entry newPilotEntry(int row_id) { return Entry(DataPosition("pilots", row_id)); }

static
bool insertPilot(UserInput& uin)
{
    DEB("Inserting...");
    auto data = uin.all();
    DataPosition position = DataPosition("pilots", 0);

    if (data.isEmpty()) {
        DEB("Object Contains no data. Aborting.");
        return false;
    }
    QString statement = "INSERT INTO " + position.first + QLatin1String(" (");
    QMap<QString, QString>::const_iterator i;
    for (i = data.cbegin(); i != data.cend(); ++i) {
        statement += i.key() + QLatin1String(", ");
    }
    statement.chop(2);
    statement += QLatin1String(") VALUES (");
    for (i = data.cbegin(); i != data.cend(); ++i) {
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

class DB {
private:
    static QVector<QString> columns;
public:
    /*!
     * \brief init: Initialise DB class and populate columns.
     */
    static bool init();

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

    static bool update(Entry updated_entry)
    {
        auto position = updated_entry.position;
        auto data = updated_entry.data();
        QString statement = "UPDATE " + position.first + " SET ";
        QMap<QString, QString>::const_iterator i;
        for (i = data.constBegin(); i != data.constEnd(); ++i) {
            if (i.key() != QString()) {
                statement += i.key() + QLatin1String("='") + i.value() + QLatin1String("', ");
            } else {
                DEB(i.key() << "is empty key. skipping.");
            }
        }
        statement.chop(2); // Remove last comma
        statement.append(QLatin1String(" WHERE _rowid_=") + QString::number(position.second));

        //execute query
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

    static
    bool verify(Entry entry)
    {
        //retreive database layout
        const auto dbContent = DbInfo();
        columns = dbContent.format.value(entry.position.first);
        TableData data = entry.data();
        auto position = entry.position;

        //Check validity of newData
        QVector<QString> badkeys;
        for (auto i = data.cbegin(); i != data.cend(); ++i) {
            if (!columns.contains(i.key())) {
                DEB(i.key() << "Not in column list for table " << position.first << ". Discarding.");
                badkeys << i.key();
            }
        }
        for (const auto &var : badkeys) {
            data.remove(var);
        }
        entry.populate_data(data);
        return update(entry);
    }

};

}  // namespace experimental

#endif
