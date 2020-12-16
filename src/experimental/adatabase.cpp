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
#include "adatabase.h"

namespace experimental {

ADataBase* ADataBase::instance = nullptr;

ADataBase* ADataBase::getInstance()
{
    if(!instance)
        instance = new ADataBase();
    return instance;
}

bool ADataBase::connect()
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

void ADataBase::disconnect()
{
    auto db = ADataBase::database();
    db.close();
    db.removeDatabase(db.connectionName());
    DEB("Database connection closed.");
}

QSqlDatabase ADataBase::database()
{
    return QSqlDatabase::database("qt_sql_default_connection");
}

bool ADataBase::commit(AEntry entry)
{
    if (exists(entry)) {
        return update(entry);
    } else {
        return insert(entry);
    }
}

bool ADataBase::remove(AEntry entry)
{
    if (!exists(entry)) {
        DEB("Error: Entry does not exist.");
        return false;
    }

    QString statement = "DELETE FROM " + entry.getPosition().tableName +
            " WHERE ROWID=" + QString::number(entry.getPosition().rowId);
    QSqlQuery query(statement);

    if (query.lastError().type() == QSqlError::NoError)
    {
        DEB("Entry " << entry.getPosition().tableName << entry.getPosition().rowId << " removed.");
        emit sqlSuccessful();
        return true;
    } else {
        DEB("Unable to delete.");
        DEB("Query: " << statement);
        DEB("Query Error: " << query.lastError().text());
        emit sqlError(query.lastError(), statement);
        return false;
    }
}

bool ADataBase::removeMany(QList<DataPosition> data_position_list)
{
    int errorCount = 0;
    QSqlQuery query;
    query.prepare("BEGIN EXCLUSIVE TRANSACTION");
    query.exec();

    for (const auto data_position : data_position_list) {
        if (!exists(data_position)) {
            DEB("Error: Entry does not exist.");
            errorCount++;
        }
        QString statement = "DELETE FROM " + data_position.first +
                " WHERE ROWID=" + QString::number(data_position.second);

        query.prepare(statement);
        query.exec();

        if (!(query.lastError().type() == QSqlError::NoError))
            errorCount++;
    }

    if (errorCount == 0) {
        query.prepare("COMMIT");
        query.exec();
        if(query.lastError().type() == QSqlError::NoError) {
            emit sqlSuccessful();
            return true;
        } else {
            emit sqlError(query.lastError(), "Transaction unsuccessful. Error count: " + QString::number(errorCount));
            return false;
        }
    } else {
        query.prepare("ROLLBACK");
        query.exec();
        emit sqlError(query.lastError(), "Transaction unsuccessful(rolled back). Error count: " + QString::number(errorCount));
        return false;
    }
}

bool ADataBase::exists(AEntry entry)
{
    if(entry.getPosition().second == 0)
        return false;

    //Check database for row id
    QString statement = "SELECT COUNT(*) FROM " + entry.getPosition().tableName +
            " WHERE ROWID=" + QString::number(entry.getPosition().rowId);
    QSqlQuery query;
    query.prepare(statement);
    query.setForwardOnly(true);
    query.exec();
    //this returns either 1 or 0 since row ids are unique
    if (!query.isActive()) {
        emit sqlError(query.lastError(), statement);
        DEB("Query Error: " << query.lastError().text() << statement);
    }
    query.next();
    int rowId = query.value(0).toInt();
    if (rowId) {
        DEB("Entry " << entry.getPosition() << " exists.");
        return true;
    } else {
        DEB("Entry does not exist.");
        return false;
    }
}

bool ADataBase::exists(DataPosition data_position)
{
    if(data_position.second == 0)
        return false;

    //Check database for row id
    QString statement = "SELECT COUNT(*) FROM " + data_position.first +
            " WHERE ROWID=" + QString::number(data_position.second);
    QSqlQuery query;
    query.prepare(statement);
    query.setForwardOnly(true);
    query.exec();
    //this returns either 1 or 0 since row ids are unique
    if (!query.isActive()) {
        emit sqlError(query.lastError(), statement);
        DEB("Query Error: " << query.lastError().text() << statement);
    }
    query.next();
    int rowId = query.value(0).toInt();
    if (rowId) {
        DEB("Entry exists at DataPosition: " << data_position);
        return true;
    } else {
        DEB("No entry exists at DataPosition: " << data_position);
        return false;
    }
}


bool ADataBase::update(AEntry updated_entry)
{
    auto data = updated_entry.getData();
    QString statement = "UPDATE " + updated_entry.getPosition().tableName + " SET ";
    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
        if (i.value() == QString()) {
            statement += i.key() + QLatin1String("=NULL") + QLatin1String(", ");
        } else {
            statement += i.key() + QLatin1String("=\"") + i.value() + QLatin1String("\", ");
        }
    }
    statement.chop(2); // Remove last comma
    statement.append(QLatin1String(" WHERE ROWID=") + QString::number(updated_entry.getPosition().rowId));

    DEB("UPDATE QUERY: " << statement);
    QSqlQuery query(statement);

    if (query.lastError().type() == QSqlError::NoError)
    {
        DEB("Entry successfully committed.");
        emit sqlSuccessful();
        return true;
    } else {
        DEB("Unable to commit.");
        DEB("Query: " << statement);
        DEB("Query Error: " << query.lastError().text());
        emit sqlError(query.lastError(), statement);
        return false;
    }
}

bool ADataBase::insert(AEntry new_entry)
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
        if (i.value() == "") {
            statement += QLatin1String("NULL, ");
        } else {
            statement += QLatin1String("\"") + i.value() + QLatin1String("\", ");
        }
    }
    statement.chop(2);
    statement += QLatin1String(")");
    DEB(statement);

    QSqlQuery query(statement);
    //check result.
    if (query.lastError().type() == QSqlError::NoError)
    {
        DEB("Entry successfully committed.");
        emit sqlSuccessful();
        return true;
    } else {
        DEB("Unable to commit.");
        DEB("Query: " << statement);
        DEB("Query Error: " << query.lastError().text());
        emit sqlError(query.lastError(), statement);
        return false;
    }

}

TableData ADataBase::getEntryData(DataPosition data_position)
{
    // check table exists
    if (!tableNames.contains(data_position.first)) {
        DEB(data_position.first << " not a table in the database. Unable to retreive Entry data.");
        return TableData();
    }

    //Check Database for rowId
    QString statement = "SELECT COUNT(*) FROM " + data_position.first
                      + " WHERE ROWID=" + QString::number(data_position.second);
    QSqlQuery check_query;
    check_query.prepare(statement);
    check_query.setForwardOnly(true);
    check_query.exec();

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
              + " WHERE ROWID=" + QString::number(data_position.second);

    QSqlQuery select_query;
    select_query.prepare(statement);
    select_query.setForwardOnly(true);
    select_query.exec();

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

AEntry ADataBase::getEntry(DataPosition data_position)
{
    AEntry entry(data_position);
    entry.setData(getEntryData(data_position));
    return entry;
}

APilotEntry ADataBase::getPilotEntry(RowId row_id)
{
    APilotEntry pilot_entry(row_id);
    pilot_entry.setData(getEntryData(pilot_entry.getPosition()));
    return pilot_entry;
}

ATailEntry ADataBase::getTailEntry(RowId row_id)
{
    ATailEntry tail_entry(row_id);
    tail_entry.setData(getEntryData(tail_entry.getPosition()));
    return tail_entry;
}

AAircraftEntry ADataBase::getAircraftEntry(RowId row_id)
{
    AAircraftEntry aircraft_entry(row_id);
    aircraft_entry.setData(getEntryData(aircraft_entry.getPosition()));
    return aircraft_entry;
}

AFlightEntry ADataBase::getFlightEntry(RowId row_id)
{
    AFlightEntry flight_entry(row_id);
    flight_entry.setData(getEntryData(flight_entry.getPosition()));
    return flight_entry;
}

const QStringList ADataBase::getCompletionList(ADataBase::DatabaseTarget target)
{
    QString statement;

    switch (target) {
    case pilots:
        statement.append("SELECT piclastname||\",\"||picfirstname FROM pilots");
        break;
    case aircraft:
        statement.append("SELECT make||\" \"||model FROM aircraft WHERE model IS NOT NULL "
                         "UNION "
                         "SELECT make||\" \"||model||\"-\"||variant FROM aircraft WHERE variant IS NOT NULL");
        break;
    case airport_identifier:
        statement.append("SELECT icao FROM airports UNION SELECT iata FROM airports");
        break;
    case registrations:
        statement.append("SELECT registration FROM tails");
        break;
    case companies:
        statement.append("SELECT company FROM pilots");
        break;
    default:
        DEB("Not a valid completer target for this function.");
        return QStringList();
    }

    QSqlQuery query;
    query.prepare(statement);
    query.setForwardOnly(true);
    query.exec();

    if(!query.isActive())
        emit sqlError(query.lastError(), statement);

    QStringList completer_list;
    while (query.next())
        completer_list.append(query.value(0).toString());

    completer_list.sort();
    completer_list.removeAll(QString(""));
    completer_list.removeDuplicates();

    return completer_list;
}

const QMap<QString, int> ADataBase::getIdMap(ADataBase::DatabaseTarget target)
{
    QString statement;

    switch (target) {
    case pilots:
        statement.append("SELECT ROWID, piclastname||\",\"||picfirstname FROM pilots");
        break;
    case aircraft:
        statement.append("SELECT ROWID, make||\" \"||model FROM aircraft WHERE model IS NOT NULL "
                         "UNION "
                         "SELECT ROWID, make||\" \"||model||\"-\"||variant FROM aircraft WHERE variant IS NOT NULL");
        break;
    case airport_identifier:
        statement.append("SELECT ROWID, icao FROM airports "
                         "UNION "
                         "SELECT ROWID, iata FROM airports WHERE iata IS NOT NULL");
        break;
    case airport_names:
        statement.append("SELECT ROWID, name FROM airports");
        break;
    case tails:
        statement.append("SELECT ROWID, registration FROM tails");
        break;
    default:
        DEB("Not a valid completer target for this function.");
        return QMap<QString, int>();
    }

    auto id_map = QMap<QString, int>();
    auto query = QSqlQuery(statement);
    if (!query.isActive()) {
        DEB("No result found. Check Query and Error.");
        DEB("Query: " << statement);
        DEB("Error: " << query.lastError().text());
        emit sqlError(query.lastError(), statement);
        return QMap<QString, int>();
    } else {
        QVector<QString> query_result;
        while (query.next()) {
            id_map.insert(query.value(1).toString(), query.value(0).toInt());
        }
        return id_map;
    }
}

QVector<QString> ADataBase::customQuery(QString statement, int return_values)
{
    QSqlQuery query(statement);
    query.exec();

    if (!query.first()) {
        DEB("No result found. Check Query and Error.");
        DEB("Error: " << query.lastError().text());
        DEB("Statement: " << statement);
        emit sqlError(query.lastError(), statement);
        return QVector<QString>();
    } else {
        query.first();
        query.previous();
        QVector<QString> result;
        while (query.next()) {
            for (int i = 0; i < return_values ; i++) {
                result.append(query.value(i).toString());
            }
        }
        emit sqlSuccessful();
        return result;
    }
}

ADataBase* aDB() { return ADataBase::getInstance(); }

}
