#include "DataBase.h"

namespace experimental {

DataBase* DataBase::instance = nullptr;

DataBase* DataBase::getInstance()
{
    return instance ?: new DataBase();
//    if(!instance)
//    return instance;
}

bool DataBase::connect()
{
    const QString driver("QSQLITE");

    if (!QSqlDatabase::isDriverAvailable(driver)) return false;

    QDir directory("data");
    QString databaseLocation = directory.filePath("logbook.db");
    QSqlDatabase db = QSqlDatabase::addDatabase(driver);
    db.setDatabaseName(databaseLocation);

    if (!db.open()) return false;

    DEB("Database connection established.");
    // Enable foreign key restrictions
    QSqlQuery query("PRAGMA foreign_keys = ON;");
    // Retreive database layout and store in member variables
    tableNames = db.tables();

    QStringList columnNames;
    for (const auto &table : tableNames) {
        columnNames.clear();
        QSqlRecord fields = db.record(table);
        for (int i = 0; i < fields.count(); i++) {
            columnNames.append(fields.field(i).name());
            tableColumns.insert(table, columnNames);
        }
    }
    return true;
}

bool DataBase::commit(Entry entry)
{
    if (exists(entry)) {
        return update(entry);
    } else {
        return insert(entry);
    }
}

bool DataBase::remove(Entry entry)
{
    if (!exists(entry)) {
        DEB("Error: Entry does not exist.");
        return false;
    }

    QString statement = "DELETE FROM " + entry.position.first +
            " WHERE _rowid_=" + QString::number(entry.position.second);
    QSqlQuery q(statement);
    //check result.
    if (q.lastError().type() == QSqlError::NoError)
    {
        DEB("Entry " << entry.position.first << entry.position.second << " removed.");
        return true;
    } else {
        DEB("Unable to delete.");
        DEB("Query: " << statement);
        DEB("Query Error: " << q.lastError().text());
        return false;
    }
}

bool DataBase::exists(Entry entry)
{
    if(entry.position == DEFAULT_PILOT_POSITION)
        return false;

    //Check database for row id
    QString statement = "SELECT COUNT(*) FROM " + entry.position.first +
            " WHERE _rowid_=" + QString::number(entry.position.second);
    //this returns either 1 or 0 since row ids are unique
    QSqlQuery q(statement);
    q.next();
    int rowId = q.value(0).toInt();
    if (rowId) {
        DEB("Entry exists with row ID: " << rowId);
        return true;
    } else {
        DEB("Entry does not exist.");
        return false;
    }
}


bool DataBase::update(Entry updated_entry)
{
    auto data = updated_entry.getData();
    QString statement = "UPDATE " + updated_entry.position.first + " SET ";
    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
        if (i.key() != QString()) {
            statement += i.key() + QLatin1String("='") + i.value() + QLatin1String("', ");
        } else {
            DEB(i.key() << "is empty key. skipping.");
        }
    }
    statement.chop(2); // Remove last comma
    statement.append(QLatin1String(" WHERE _rowid_=") + QString::number(updated_entry.position.second));

    DEB("UPDATE QUERY: " << statement);
    QSqlQuery q(statement);
    //check result.
    if (q.lastError().type() == QSqlError::NoError)
    {
        DEB("Entry successfully committed.");
        return true;
    } else {
        DEB("Unable to commit.");
        DEB("Query: " << statement);
        DEB("Query Error: " << q.lastError().text());
        return false;
    }
}

bool DataBase::insert(Entry newEntry)
{
    auto data = newEntry.getData();
    DEB("Inserting...");
    QString statement = "INSERT INTO " + newEntry.position.first + QLatin1String(" (");
    QMap<QString, QString>::iterator i;
    for (i = data.begin(); i != data.end(); ++i) {
        statement += i.key() + QLatin1String(", ");
    }
    statement.chop(2);
    statement += QLatin1String(") VALUES (");
    for (i = data.begin(); i != data.end(); ++i) {
        statement += QLatin1String("'") + i.value() + QLatin1String("', ");
    }
    statement.chop(2);
    statement += QLatin1String(")");

    QSqlQuery q(statement);
    //check result.
    if (q.lastError().type() == QSqlError::NoError)
    {
        DEB("Entry successfully committed.");
        return true;
    } else {
        DEB("Unable to commit.");
        DEB("Query: " << statement);
        DEB("Query Error: " << q.lastError().text());
        return false;
    }

}

DataBase* DB() { return DataBase::getInstance(); }

}
