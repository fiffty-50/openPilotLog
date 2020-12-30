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
#include "src/testing/adebug.h"

#define DATABASE_VERSION 15
const auto SQL_DRIVER = QStringLiteral("QSQLITE");

ADatabaseError::ADatabaseError(QString msg_)
    : QSqlError::QSqlError(msg_)
{}

QString ADatabaseError::text() const
{
    return "Database Error: " + QSqlError::text();
}

ADatabase* ADatabase::instance = nullptr;

TableNames ADatabase::getTableNames() const
{
    return tableNames;
}

TableColumns ADatabase::getTableColumns() const
{
    return tableColumns;
}

ADatabase* ADatabase::getInstance()
{
    if(!instance)
        instance = new ADatabase();
    return instance;
}

/*!
 * \brief ADatabase::sqliteVersion returns database sqlite version.
 * \return sqlite version string
 */
const QString ADatabase::sqliteVersion()
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT sqlite_version()"));
    query.exec();
    query.next();
    return query.value(0).toString();
}

bool ADatabase::connect()
{
    if (!QSqlDatabase::isDriverAvailable(SQL_DRIVER))
        return false;

    QDir directory(QStringLiteral("data"));
    QString databaseLocation = directory.filePath(QStringLiteral("logbook.db"));
    QSqlDatabase db = QSqlDatabase::addDatabase(SQL_DRIVER);
    db.setDatabaseName(databaseLocation);

    if (!db.open())
        return false;

    DEB "Database connection established." << db.lastError().text();
    // Enable foreign key restrictions
    QSqlQuery query(QStringLiteral("PRAGMA foreign_keys = ON;"));
    tableNames = db.tables();

    QStringList column_names;
    for (const auto &table : tableNames) {
        column_names.clear();
        QSqlRecord fields = db.record(table);
        for (int i = 0; i < fields.count(); i++) {
            column_names.append(fields.field(i).name());
            tableColumns.insert(table, column_names);
        }
    }
    DEB "Database Tables: " << tableNames;
    DEB "Tables and Columns: " << tableColumns;
    return true;
}

void ADatabase::disconnect()
{
    auto db = ADatabase::database();
    db.close();
    DEB "Database connection closed.";
}

QSqlDatabase ADatabase::database()
{
    return QSqlDatabase::database(QStringLiteral("qt_sql_default_connection"));
}

bool ADatabase::commit(AEntry entry)
{
    if (exists(entry)) {
        return update(entry);
    } else {
        return insert(entry);
    }
}

bool ADatabase::remove(AEntry entry)
{
    if (!exists(entry)) {
        DEB "Error: Database entry not found.";
        lastError = ADatabaseError(QStringLiteral("Database entry not found."));
        return false;
    }

    QString statement = "DELETE FROM " + entry.getPosition().tableName +
            " WHERE ROWID=?";

    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(entry.getPosition().rowId);
    query.exec();

    if (query.lastError().type() == QSqlError::NoError)
    {
        DEB "Entry " << entry.getPosition() << " removed.";
        emit dataBaseUpdated();
        lastError = QString();
        return true;
    } else {
        DEB "Unable to delete.";
        DEB "Query: " << statement;
        DEB "Query Error: " << query.lastError().text();
        lastError = query.lastError().text();
        return false;
    }
}

bool ADatabase::removeMany(QList<DataPosition> data_position_list)
{
    int errorCount = 0;
    QSqlQuery query;
    query.prepare(QStringLiteral("BEGIN EXCLUSIVE TRANSACTION"));
    query.exec();

    for (const auto& data_position : data_position_list) {
        if (!exists(data_position)) {
            lastError = ADatabaseError("Database entry not found.");
            errorCount++;
        }
        QString statement = "DELETE FROM " + data_position.tableName +
                " WHERE ROWID=?";

        query.prepare(statement);
        query.addBindValue(data_position.rowId);
        query.exec();

        if (!(query.lastError().type() == QSqlError::NoError))
            errorCount++;
    }

    if (errorCount == 0) {
        query.prepare(QStringLiteral("COMMIT"));
        query.exec();
        if(query.lastError().type() == QSqlError::NoError) {
            emit dataBaseUpdated();
            lastError = QString();
            return true;
        } else {
            lastError = "Transaction unsuccessful (Interrupted). Error count: " + QString::number(errorCount);
            return false;
        }
    } else {
        query.prepare(QStringLiteral("ROLLBACK"));
        query.exec();
        lastError = "Transaction unsuccessful (no changes have been made). Error count: " + QString::number(errorCount);
        return false;
    }
}

bool ADatabase::exists(AEntry entry)
{
    if(entry.getPosition().rowId == 0)
        return false;

    //Check database for row id
    QString statement = "SELECT COUNT(*) FROM " + entry.getPosition().tableName +
            " WHERE ROWID=?";
    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(entry.getPosition().rowId);
    query.setForwardOnly(true);
    query.exec();
    //this returns either 1 or 0 since row ids are unique
    if (!query.isActive()) {
        lastError = query.lastError().text();
        DEB "Query Error: " << query.lastError().text() << statement;
        return false;
    }
    query.next();
    int rowId = query.value(0).toInt();
    if (rowId) {
        return true;
    } else {
        DEB "Database entry not found.";
        lastError = ADatabaseError(QStringLiteral("Database entry not found."));
        return false;
    }
}

bool ADatabase::exists(DataPosition data_position)
{
    if(data_position.rowId == 0)
        return false;

    //Check database for row id
    QString statement = "SELECT COUNT(*) FROM " + data_position.tableName +
            " WHERE ROWID=?";
    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(data_position.rowId);
    query.setForwardOnly(true);
    query.exec();
    //this returns either 1 or 0 since row ids are unique
    if (!query.isActive()) {
        lastError = query.lastError().text();
        DEB "Query Error: " << query.lastError().text() << statement;
    }
    query.next();
    int rowId = query.value(0).toInt();
    if (rowId) {
        return true;
    } else {
        DEB "No entry exists at DataPosition: " << data_position.tableName << data_position.rowId;
        lastError = ADatabaseError(QStringLiteral("Database entry not found."));
        return false;
    }
}


bool ADatabase::update(AEntry updated_entry)
{
    auto data = updated_entry.getData();
    QString statement = "UPDATE " + updated_entry.getPosition().tableName + " SET ";
    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
        statement.append(i.key() + "=?,");
    }
    statement.chop(1);
    statement.append(" WHERE ROWID=?");
    QSqlQuery query;
    query.prepare(statement);
    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
        if (i.value() == QVariant(QVariant::String)) {
            query.addBindValue(QVariant(QVariant::String));
        } else {
            query.addBindValue(i.value());
        }
    }
    query.addBindValue(updated_entry.getPosition().rowId);
    query.exec();

    if (query.lastError().type() == QSqlError::NoError)
    {
        DEB "Entry successfully committed.";
        emit dataBaseUpdated();
        lastError = QString();
        return true;
    } else {
        DEB "Unable to commit.";
        DEB "Query: " << statement;
        DEB "Query Error: " << query.lastError().text();
        lastError = query.lastError().text();
        return false;
    }
}

bool ADatabase::insert(AEntry new_entry)
{
    auto data = new_entry.getData();
    QString statement = "INSERT INTO " + new_entry.getPosition().tableName + QLatin1String(" (");
    QMap<QString, QVariant>::iterator i;
    for (i = data.begin(); i != data.end(); ++i) {
        statement.append(i.key() + ',');
    }
    statement.chop(1);
    statement += QLatin1String(") VALUES (");

    for (int i=0; i < data.size(); ++i) {
        statement += QLatin1String("?,");
    }
    statement.chop(1);
    statement += ')';

    QSqlQuery query;
    query.prepare(statement);

    for (i = data.begin(); i != data.end(); ++i) {
        if (i.value() == "") {
            query.addBindValue(QVariant(QVariant::String));
        } else {
            query.addBindValue(i.value());
        }
    }

    query.exec();
    //check result.
    if (query.lastError().type() == QSqlError::NoError)
    {
        DEB "Entry successfully committed.";
        emit dataBaseUpdated();
        lastError = QString();
        return true;
    } else {
        DEB "Unable to commit.";
        DEB "Query: " << statement;
        DEB "Query Error: " << query.lastError().text();
        lastError = query.lastError().text();
        return false;
    }

}

RowData ADatabase::getEntryData(DataPosition data_position)
{
    // check table exists
    if (!tableNames.contains(data_position.tableName)) {
        DEB data_position.tableName << " not a table in the database. Unable to retreive Entry data.";
        return RowData();
    }

    //Check Database for rowId
    QString statement = "SELECT COUNT(*) FROM " + data_position.tableName
                      + " WHERE ROWID=?";
    QSqlQuery check_query;
    check_query.prepare(statement);
    check_query.addBindValue(data_position.rowId);
    check_query.setForwardOnly(true);
    check_query.exec();

    if (check_query.lastError().type() != QSqlError::NoError) {
        DEB "SQL error: " << check_query.lastError().text();
        DEB "Statement: " << statement;
        lastError = check_query.lastError().text();
        return RowData();
    }

    check_query.next();
    if (check_query.value(0).toInt() == 0) {
        DEB "No Entry found for row id: " << data_position.rowId;
        lastError = ADatabaseError("Database entry not found.");
        return RowData();
    }

    // Retreive TableData
    statement = "SELECT * FROM " + data_position.tableName
              + " WHERE ROWID=?";

    QSqlQuery select_query;
    select_query.prepare(statement);
    select_query.addBindValue(data_position.rowId);
    select_query.setForwardOnly(true);
    select_query.exec();

    if (select_query.lastError().type() != QSqlError::NoError) {
        DEB "SQL error: " << select_query.lastError().text();
        DEB "Statement: " << statement;
        lastError = select_query.lastError().text();
        return RowData();
    }

    select_query.next();
    RowData entry_data;

    for (const auto &column : tableColumns.value(data_position.tableName)) {
        entry_data.insert(column, select_query.value(column));
    }
    return entry_data;
}

AEntry ADatabase::getEntry(DataPosition data_position)
{
    AEntry entry(data_position);
    entry.setData(getEntryData(data_position));
    return entry;
}

APilotEntry ADatabase::getPilotEntry(RowId row_id)
{
    APilotEntry pilot_entry(row_id);
    pilot_entry.setData(getEntryData(pilot_entry.getPosition()));
    return pilot_entry;
}

ATailEntry ADatabase::getTailEntry(RowId row_id)
{
    ATailEntry tail_entry(row_id);
    tail_entry.setData(getEntryData(tail_entry.getPosition()));
    return tail_entry;
}

AAircraftEntry ADatabase::getAircraftEntry(RowId row_id)
{
    AAircraftEntry aircraft_entry(row_id);
    aircraft_entry.setData(getEntryData(aircraft_entry.getPosition()));
    return aircraft_entry;
}

AFlightEntry ADatabase::getFlightEntry(RowId row_id)
{
    AFlightEntry flight_entry(row_id);
    flight_entry.setData(getEntryData(flight_entry.getPosition()));
    return flight_entry;
}

const QStringList ADatabase::getCompletionList(ADatabaseTarget target)
{
    QString statement;

    switch (target) {
    case ADatabaseTarget::pilots:
        statement.append(QStringLiteral("SELECT lastname||', '||firstname FROM pilots"));
        break;
    case ADatabaseTarget::aircraft:
        statement.append(QStringLiteral("SELECT make||' '||model FROM aircraft WHERE model IS NOT NULL "
                         "UNION "
                         "SELECT make||' '||model||'-'||variant FROM aircraft WHERE variant IS NOT NULL"));
        break;
    case ADatabaseTarget::airport_identifier_all:
        statement.append(QStringLiteral("SELECT icao FROM airports UNION SELECT iata FROM airports"));
        break;
    case ADatabaseTarget::registrations:
        statement.append(QStringLiteral("SELECT registration FROM tails"));
        break;
    case ADatabaseTarget::companies:
        statement.append(QStringLiteral("SELECT company FROM pilots"));
        break;
    default:
        DEB "Not a valid completer target for this function.";
        return QStringList();
    }

    QSqlQuery query;
    query.prepare(statement);
    query.setForwardOnly(true);
    query.exec();

    if(!query.isActive()) {
        lastError = query.lastError().text();
        return QStringList();
    }

    QStringList completer_list;
    while (query.next())
        completer_list.append(query.value(0).toString());

    completer_list.sort();
    completer_list.removeAll(QString(""));
    completer_list.removeDuplicates();

    return completer_list;
}

const QMap<QString, int> ADatabase::getIdMap(ADatabaseTarget target)
{
    QString statement;

    switch (target) {
    case ADatabaseTarget::pilots:
        statement.append(QStringLiteral("SELECT ROWID, lastname||', '||firstname FROM pilots"));
        break;
    case ADatabaseTarget::aircraft:
        statement.append(QStringLiteral("SELECT ROWID, make||' '||model FROM aircraft WHERE model IS NOT NULL "
                         "UNION "
                         "SELECT ROWID, make||' '||model||'-'||variant FROM aircraft WHERE variant IS NOT NULL"));
        break;
    case ADatabaseTarget::airport_identifier_icao:
        statement.append(QStringLiteral("SELECT ROWID, icao FROM airports"));
        break;
    case ADatabaseTarget::airport_identifier_iata:
        statement.append(QStringLiteral("SELECT ROWID, iata FROM airports WHERE iata NOT NULL"));
        break;
    case ADatabaseTarget::airport_names:
        statement.append(QStringLiteral("SELECT ROWID, name FROM airports"));
        break;
    case ADatabaseTarget::tails:
        statement.append(QStringLiteral("SELECT ROWID, registration FROM tails"));
        break;
    default:
        DEB "Not a valid completer target for this function.";
        return QMap<QString, int>();
    }

    auto id_map = QMap<QString, int>();
    auto query = QSqlQuery(statement);
    if (!query.isActive()) {
        DEB "No result found. Check Query and Error.";
        DEB "Query: " << statement;
        DEB "Error: " << query.lastError().text();
        lastError = query.lastError().text();
        return QMap<QString, int>();
    } else {
        QVector<QString> query_result;
        while (query.next()) {
            id_map.insert(query.value(1).toString(), query.value(0).toInt());
        }
        return id_map;
    }
}

int ADatabase::getLastEntry(ADatabaseTarget target)
{
    QString statement = "SELECT MAX(ROWID) FROM ";

    switch (target) {
    case ADatabaseTarget::pilots:
        statement.append(DB_TABLE_PILOTS);
        break;
    case ADatabaseTarget::aircraft:
        statement.append(DB_TABLE_AIRCRAFT);
        break;
    case ADatabaseTarget::tails:
        statement.append(DB_TABLE_TAILS);
        break;
    default:
        DEB "Not a valid completer target for this function.";
        return 0;
    }
    auto query = QSqlQuery(statement);
    if (query.first()) {
        return query.value(0).toInt();
    } else {
        lastError = ADatabaseError(QStringLiteral("Database entry not found."));
        DEB "No entry found.";
        return 0;
    }
}

QList<int> ADatabase::getForeignKeyConstraints(int foreign_row_id, ADatabaseTarget target)
{
    QString statement = "SELECT ROWID FROM flights WHERE ";

    switch (target) {
    case ADatabaseTarget::pilots:
        statement.append("pic=?");
        break;
    case ADatabaseTarget::tails:
        statement.append("acft=?");
        break;
    default:
        DEB "Not a valid target for this function.";
        return QList<int>();
        break;
    }

    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(foreign_row_id);
    query.exec();

    if (!query.isActive()) {
        lastError = query.lastError().text();
        DEB "Error";
        DEB statement;
        DEB query.lastError().text();
        return QList<int>();
    }

    QList<int> row_ids;
    while (query.next()) {
        row_ids.append(query.value(0).toInt());
    }
    return row_ids;
}

APilotEntry ADatabase::resolveForeignPilot(int foreign_key)
{
    return aDB()->getPilotEntry(foreign_key);
}

ATailEntry ADatabase::resolveForeignTail(int foreign_key)
{
    return aDB()->getTailEntry(foreign_key);
}

QVector<QString> ADatabase::customQuery(QString statement, int return_values)
{
    QSqlQuery query(statement);
    query.exec();

    if (!query.first()) {
        DEB "No result found. Check Query and Error.";
        DEB "Error: " << query.lastError().text();
        DEB "Statement: " << statement;
        lastError = query.lastError().text();
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
        emit dataBaseUpdated();
        lastError = QString();
        return result;
    }
}

ADatabase* aDB() { return ADatabase::getInstance(); }
