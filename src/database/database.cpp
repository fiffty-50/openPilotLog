/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#include "database.h"
#include "queryfactory.h"
#include "src/opl.h"
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QtSql/qsqlquery.h>
#include <utility>

namespace OPL {

bool Database::connect()
{
    if (!QSqlDatabase::isDriverAvailable(SQLITE_DRIVER)) {
        LOG << "Error: No SQLITE Driver availabe.";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(SQLITE_DRIVER);
    db.setDatabaseName(m_databaseFile.absoluteFilePath());

    if (!db.open()) {
        LOG << QStringLiteral("Unable to establish database connection.<br>The following error has "
                              "ocurred:<br><br>%1")
                   .arg(db.lastError().databaseText());
        m_lastError = db.lastError();
        return false;
    }

    LOG << "Database connection established: " + m_databaseFile.absoluteFilePath();
    // Enable foreign key restrictions
    QSqlQuery query;
    query.prepare(QStringLiteral("PRAGMA foreign_keys = ON;"));
    query.exec();
    updateLayout();
    return true;
}

void Database::disconnect()
{
    auto db = database();
    db.close();
    QSqlDatabase::removeDatabase(CONNECTION_NAME);
    LOG << "Database connection closed: " << CONNECTION_NAME;
}

QStringList Database::getTableColumns(OPL::DbTable table_name) const
{
    return m_tableColumns.value(tableName(table_name));
}

QStringList Database::getTableNames() const { return m_tableNames; }

void Database::updateLayout()
{
    auto db      = Database::database();
    m_tableNames = db.tables();

    m_tableColumns.clear();
    for (const auto &table_name : std::as_const(m_tableNames)) {
        QStringList table_columns;
        QSqlRecord fields = db.record(table_name);
        for (int i = 0; i < fields.count(); i++) {
            table_columns.append(fields.field(i).name());
        }
        m_tableColumns.insert(table_name, table_columns);
    }
    emit databaseUpdated(DbTable::Any);
}

bool Database::exec(QSqlQuery &q, DbTable table)
{
    if (!q.exec()) {
        LOG << "Query failed: " << q.lastQuery();
        DEB << "Values" << q.boundValues();
        LOG << q.lastError().text();
        m_lastError = q.lastError();
        return false;
    }

    emit databaseUpdated(table);
    return true;
}
bool Database::execQuietly(QSqlQuery &q)
{
    if (!q.exec()) {
        LOG << "Query failed: " << q.lastQuery();
        DEB << "Values" << q.boundValues();
        LOG << q.lastError().text();
        m_lastError = q.lastError();
        return false;
    }
    return true;
}

bool Database::commit(const OPL::Row &row)
{
    if (!row.isValid()) return false;

    if (exists(row))
        return update(row);
    else
        return insert(row);
}

bool Database::commit(const QJsonArray &json_arr, const OPL::DbTable table)
{
    // start a transaction to avoid disk writes in every loop
    database().transaction();

    // build the query once and only replace bound vaues in between exec calls
    QSqlQuery q = QUERIES->insert(table, getTableColumns(table));

    // iterate over the JSON data and commit each objects data
    for (const auto &entry : json_arr) {
        auto object     = entry.toObject();
        const auto keys = object.keys();
        for (const auto &key : keys) {
            auto value       = object.value(key).toVariant();
            auto placeholder = QStringLiteral(":").append(key);
            if (!value.isNull()) {
                q.bindValue(placeholder, value);
            }
            else {
                // explicitly bind NULL
                q.bindValue(placeholder, QVariant(value.metaType()));
            }

            execQuietly(q);
        }
    }

    // write to disk
    if (!database().commit()) {
        LOG << "Import of JSON data has failed.";
        database().rollback();
        return false;
    }

    emit databaseUpdated(table);
    return true;
}

bool Database::commit(FlightDataBuilder &builder)
{
    if (!builder.validate()) {
        LOG << "Unable to validate flight data.";
        DEB << builder.errors();
        return false;
    }
    if (exists(DbTable::LogEvents, builder.eventId())) {
        return update(builder);
    }
    else {
        return insert(builder);
    }
}

bool Database::remove(const OPL::Row &row)
{
    if (!exists(row)) {
        LOG << "Unable to delete: Database entry not found.";
        DEB << row;
        return false;
    }

    QSqlQuery q = QUERIES->deleteFrom(row.getTable(), row.getRowId());
    return exec(q);
}

bool Database::remove(OPL::DbTable table, int row_id)
{
    if (!exists(table, row_id)) {
        LOG << "Unable to delete: Database entry not found.";
        DEB << tableName(table) << '/' << row_id;
        return false;
    }

    QSqlQuery q = QUERIES->deleteFrom(table, row_id);
    return exec(q);
}

bool Database::exists(const OPL::Row &row)
{
    const int row_id = row.getRowId();
    if (row_id == 0) return false;

    auto q = QUERIES->exists(row);
    if (!execQuietly(q)) return false;

    return q.next(); // true only if a row exists
}

bool Database::exists(DbTable table, int row_id)
{
    if (row_id == 0) return false;

    auto q = QUERIES->exists(table, row_id);
    if (!execQuietly(q)) return false;

    return q.next(); // true only if a row exists
}

bool Database::update(const OPL::Row &updated_row)
{
    QSqlQuery q = QUERIES->update(updated_row);
    if (exec(q, updated_row.getTable())) {

        LOG << QString("Entry successfully updated. %1").arg(updated_row.getPosition());
        return true;
    }
    return false;
}

bool Database::insert(const OPL::Row &new_row)
{
    QSqlQuery q = QUERIES->insert(new_row);
    if (exec(q, new_row.getTable())) {

        LOG << QString("Entry successfully committed. %1").arg(new_row.getPosition());
        return true;
    }
    return false;
}

bool Database::insert(FlightDataBuilder &builder)
{
    // start a transaction
    database().transaction();

    // LogEntry, FlightLogEntry are mandatory, rollback on failure.
    // LogEntry
    auto log_entry = builder.logEntry();
    DEB << "Commiting LOG:" << log_entry;
    if (!commit(log_entry)) {
        database().rollback();
        return false;
    }
    builder.setEventId(getLastEntry(DbTable::LogEvents));

    // FlightLogEntry
    auto flight_entry = builder.flightLogEntry();
    DEB << "Commiting FLT:" << flight_entry;
    if (!commit(flight_entry)) {
        database().rollback();
        return false;
    }
    builder.setFlightId(getLastEntry(DbTable::Flights));

    // Flight Segment Entries
    auto flight_segments = builder.flightSegments();
    for (const auto &s : std::as_const(flight_segments)) {
        DEB << "Commiting Segment:" << s;
        if (!commit(s)) {
            database().rollback();
            return false;
        }
    }

    // Optional part, don't rollback since those may be empty
    // Movements
    auto movements = builder.movements();
    for (const auto &m : std::as_const(movements)) {
        DEB << "Commiting Movement:" << m;
        commit(m);
    }

    // Approaches
    auto approaches = builder.approaches();
    for (const auto &a : std::as_const(approaches)) {
        DEB << "Commiting Approach:" << a;
        commit(a);
    }

    // commit the transaction
    if (!database().commit()) {
        m_lastError = database().lastError();
        return false;
    }
    return true;
}

bool Database::update(FlightDataBuilder &flight_data)
{
    // helper function to clear entries that are built from scratch
    auto deleteWhere = [this](DbTable table, const QString &column_name, int value) -> bool {
        QSqlQuery q = QUERIES->deleteFromWhere(table, column_name, value);
        return execQuietly(q);
    };
    auto static S_COL_FLIGHT_ID = QStringLiteral("flight_id");
    auto static S_COL_EVENT_ID  = QStringLiteral("event_id");

    // start a transaction to enable rollback on failure
    database().transaction();

    // Update LogEntry
    auto log_entry = flight_data.logEntry();
    DEB << "Updating LOG:" << log_entry;
    if (!commit(log_entry)) {
        database().rollback();
        return false;
    }

    // Update FlightLogEntry
    auto flight_entry = flight_data.flightLogEntry();
    DEB << "Commiting FLT:" << flight_entry;
    if (!commit(flight_entry)) {
        database().rollback();
        return false;
    }

    // Delete and re-enter those values instead of updating
    // Flight Segment Entries
    if (!deleteWhere(DbTable::FlightSegments, S_COL_FLIGHT_ID, flight_data.flightId())) {
        database().rollback();
        return false;
    }

    auto flight_segments = flight_data.flightSegments();
    for (const auto &s : std::as_const(flight_segments)) {
        DEB << "Commiting Segment:" << s;
        if (!commit(s)) {
            database().rollback();
            return false;
        }
    }

    // Optional part, don't rollback since those may be empty
    // Movements
    if (!deleteWhere(DbTable::MovementEvents, S_COL_EVENT_ID, flight_data.eventId())) {
        return false;
    }

    auto movements = flight_data.movements();
    for (const auto &m : std::as_const(movements)) {
        DEB << "Commiting Movement:" << m;
        commit(m);
    }

    // Approaches
    if (!deleteWhere(DbTable::ApproachEvents, S_COL_EVENT_ID, flight_data.eventId())) {
        return false;
    }

    auto approaches = flight_data.approaches();
    for (const auto &a : std::as_const(approaches)) {
        DEB << "Commiting Approach:" << a;
        commit(a);
    }

    // commit the transaction
    DEB << "All flight elements processed. Commiting transaction";
    if (!database().commit()) {
        m_lastError = database().lastError();
        return false;
    }
    return true;
}

RowData_T Database::getRowData(const OPL::DbTable table, const int row_id)
{
    QSqlQuery q = QUERIES->selectFrom(table, row_id);
    execQuietly(q);

    RowData_T entry_data;
    if (q.next()) {
        auto r = q.record(); // retreive record
        if (r.count() == 0)  // row is empty
            return {};

        for (int i = 0; i < r.count(); i++) { // iterate through fields to get key:value map
            if (!r.value(i).isNull()) {
                entry_data.insert(r.fieldName(i), r.value(i));
            }
        }
    }

    return entry_data;
}

RowData_T Database::getRowData(const OPL::DbTable table, const QString &filter_column, int row_id)
{
    QSqlQuery q = QUERIES->selectFromWhere(table, filter_column, row_id);
    execQuietly(q);

    RowData_T entry_data;
    if (q.next()) {
        auto r = q.record(); // retreive record
        if (r.count() == 0)  // row is empty
            return {};

        for (int i = 0; i < r.count(); i++) { // iterate through fields to get key:value map
            if (!r.value(i).isNull()) {
                entry_data.insert(r.fieldName(i), r.value(i));
            }
        }
    }

    return entry_data;
}
QList<RowData_T> Database::getRowsData(const DbTable table, const QString &filterColumn, int row_id)
{
    QSqlQuery q = QUERIES->selectFromWhere(table, filterColumn, row_id);
    if (!execQuietly(q)) return {};

    QList<RowData_T> rows;
    while (q.next()) {
        auto r = q.record(); // retreive record
        if (r.count() == 0)  // row is empty
            continue;

        RowData_T entry_data;
        for (int i = 0; i < r.count(); i++) { // iterate through fields to get key:value map
            if (!r.value(i).isNull()) {
                entry_data.insert(r.fieldName(i), r.value(i));
            }
        }

        if (!entry_data.isEmpty()) {
            rows.append(entry_data);
        }
    }

    return rows;
}

int Database::getLastEntry(OPL::DbTable table)
{
    QString statement = QLatin1String("SELECT MAX(ROWID) FROM ") + tableName(table);

    auto query = QSqlQuery(statement);
    if (query.first()) {
        return query.value(0).toInt();
    }
    else {
        LOG << "No entry found. (Database empty?)" << query.lastError().text();
        return 0;
    }
}

QList<int> Database::getForeignKeyConstraints(int foreign_row_id, OPL::DbTable table)
{
    QString column;
    switch (table) {
    case OPL::DbTable::Pilots:
        column = QStringLiteral("pic");
        break;
    case OPL::DbTable::AircraftTails:
        column = QStringLiteral("tail_id");
        break;
    case OPL::DbTable::AircraftTypes:
        column = QStringLiteral("aircraft_type_id=?");
        break;
    default:
        DEB << "Not a valid target for this function.";
        assert(false);
        return QList<int>();
        break;
    }

    QSqlQuery q = QUERIES->getForeignKeyConstraint(table, column, foreign_row_id);
    execQuietly(q);

    QList<int> row_ids;
    while (q.next()) {
        row_ids.append(q.value(0).toInt());
    }
    return row_ids;
}

QVector<RowData_T> Database::getTable(OPL::DbTable table)
{
    QSqlQuery q = QUERIES->getTable(table);

    QVector<RowData_T> entry_data;
    while (q.next()) { // iterate through records
        auto r = q.record();
        // DEB << r;
        RowData_T row;
        for (int i = 0; i < r.count(); i++) {
            if (!r.value(i).isNull()) {
                row.insert(r.fieldName(i), r.value(i));
            }
        }
        entry_data.append(row);
    }
    return entry_data;
}

bool Database::createBackup(const QString &dest_file)
{
    LOG << "Backing up current database to: " << dest_file;
    Database::disconnect();
    QFile db_file(QDir::toNativeSeparators(m_databaseFile.absoluteFilePath()));

    if (!db_file.copy(QDir::toNativeSeparators(dest_file))) {
        LOG << "Unable to backup old database:" << db_file.errorString();
        return false;
    }

    LOG << "Backed up old database as:" << dest_file;
    Database::connect();
    emit connectionReset();
    return true;
}

bool Database::restoreBackup(const QString &backup_file)
{
    Database::disconnect();
    LOG << "Restoring backup from file:" << backup_file;

    QString databaseFilePath = QDir::toNativeSeparators(m_databaseFile.absoluteFilePath());
    DEB << "DB File Path: " << databaseFilePath;
    QString backupFilePath = QDir::toNativeSeparators(backup_file);

    QFile dbFile(databaseFilePath);
    if (dbFile.exists())
        if (!dbFile.remove()) {
            LOG << dbFile.errorString() << "Unable to remove current db file";
            return false;
        }

    QFile backupFile(backupFilePath);
    if (!backupFile.copy(databaseFilePath)) {
        LOG << backupFile.errorString() << "Could not copy" << backupFile.fileName() << " to "
            << databaseFilePath;
        return false;
    }

    LOG << "Backup successfully restored!";
    Database::connect();
    emit connectionReset();
    return true;
}

} // namespace OPL
