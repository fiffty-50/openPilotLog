#include "DataBase.h"

namespace experimental {

DataBase* DataBase::instance = nullptr;

DataBase* DataBase::getInstance()
{
    if(!instance)
        instance = new DataBase();
    return instance;
}

bool DataBase::connect()
{
    const QString driver("QSQLITE");

    if (!QSqlDatabase::isDriverAvailable(driver))
        return false;

    QDir directory("data");
    QString databaseLocation = directory.filePath("logbook.db");
    QSqlDatabase db = QSqlDatabase::addDatabase(driver);
    db.setDatabaseName(databaseLocation);

    if (!db.open())
        return false;

    DEB("Database connection established.");
    // Enable foreign key restrictions
    QSqlQuery query("PRAGMA foreign_keys = ON;");
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
    DEB("Database Tables: " << tableNames);
    return true;
}

void DataBase::disconnect()
{
    auto db = DataBase::database();
    db.close();
    db.removeDatabase(db.connectionName());
    DEB("Database connection closed.");
}

QSqlDatabase DataBase::database()
{
    return QSqlDatabase::database("qt_sql_default_connection");
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

    QString statement = "DELETE FROM " + entry.getPosition().tableName +
            " WHERE _rowid_=" + QString::number(entry.getPosition().rowId);
    QSqlQuery q(statement);

    if (q.lastError().type() == QSqlError::NoError)
    {
        DEB("Entry " << entry.getPosition().tableName << entry.getPosition().rowId << " removed.");
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
    if(entry.getPosition() == DEFAULT_PILOT_POSITION)
        return false;

    //Check database for row id
    QString statement = "SELECT COUNT(*) FROM " + entry.getPosition().tableName +
            " WHERE _rowid_=" + QString::number(entry.getPosition().rowId);
    //this returns either 1 or 0 since row ids are unique
    QSqlQuery q(statement);
    q.next();
    int rowId = q.value(0).toInt();
    if (rowId) {
        DEB("Entry " << entry.getPosition() << " exists.");
        return true;
    } else {
        DEB("Entry does not exist.");
        return false;
    }
}


bool DataBase::update(Entry updated_entry)
{
    auto data = updated_entry.getData();
    QString statement = "UPDATE " + updated_entry.getPosition().tableName + " SET ";
    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
        if (i.key() != QString()) {
            statement += i.key() + QLatin1String("='") + i.value() + QLatin1String("', ");
        } else {
            DEB(i.key() << "is empty key. skipping.");
        }
    }
    statement.chop(2); // Remove last comma
    statement.append(QLatin1String(" WHERE _rowid_=") + QString::number(updated_entry.getPosition().rowId));

    DEB("UPDATE QUERY: " << statement);
    QSqlQuery q(statement);

    if (q.lastError().type() == QSqlError::NoError)
    {
        DEB("Entry successfully committed.");
        emit commitSuccessful();
        return true;
    } else {
        DEB("Unable to commit.");
        DEB("Query: " << statement);
        DEB("Query Error: " << q.lastError().text());
        emit commitUnsuccessful(q.lastError(), statement);
        return false;
    }
}

bool DataBase::insert(Entry new_entry)
{
    auto data = new_entry.getData();
    DEB("Inserting...");
    QString statement = "INSERT INTO " + new_entry.getPosition().tableName + QLatin1String(" (");
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
        emit commitSuccessful();
        return true;
    } else {
        DEB("Unable to commit.");
        DEB("Query: " << statement);
        DEB("Query Error: " << q.lastError().text());
        emit commitUnsuccessful(q.lastError(), statement);
        return false;
    }

}

TableData DataBase::getEntryData(DataPosition data_position)
{
    // check table exists
    if (!tableNames.contains(data_position.first)) {
        DEB(data_position.first << " not a table in the database. Unable to retreive Entry data.");
        return TableData();
    }

    //Check Database for rowId
    QString statement = "SELECT COUNT(*) FROM " + data_position.first
                      + " WHERE _rowid_=" + QString::number(data_position.second);
    QSqlQuery check_query(statement);

    if (check_query.lastError().type() != QSqlError::NoError) {
        DEB("SQL error: " << check_query.lastError().text());
        DEB("Statement: " << statement);
        return TableData();
    }

    check_query.next();
    if (check_query.value(0).toInt() == 0) {
        DEB("No Entry found for row id: " << data_position.second );
        return TableData();
    }

    // Retreive TableData
    DEB("Retreiving data for row id: " << data_position.second);
    statement = "SELECT * FROM " + data_position.first
              + " WHERE _rowid_=" + QString::number(data_position.second);

    QSqlQuery select_query(statement);
    if (select_query.lastError().type() != QSqlError::NoError) {
        DEB("SQL error: " << select_query.lastError().text());
        DEB("Statement: " << statement);
        return TableData();
    }

    select_query.next();
    TableData entry_data;

    for (const auto &column : tableColumns.value(data_position.first)) {
        entry_data.insert(column, select_query.value(column).toString());
    }
    return entry_data;
}

Entry DataBase::getEntry(DataPosition data_position)
{
    Entry entry(data_position);
    entry.setData(getEntryData(data_position));
    return entry;
}

PilotEntry DataBase::getPilotEntry(RowId row_id)
{
    PilotEntry pilotEntry(row_id);
    pilotEntry.setData(getEntryData(pilotEntry.getPosition()));
    return pilotEntry;
}

DataBase* DB() { return DataBase::getInstance(); }

}
