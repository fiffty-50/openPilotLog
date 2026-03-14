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
#include "src/database/entries/flightlogentry.h"
#include "src/opl.h"
#include <QtSql/qsqlquery.h>
#include <qdir.h>
#include <qsqlerror.h>
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
        LOG << QString("Unable to establish database connection.<br>The following error has "
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
    QString connection_name;
    {
        auto db         = Database::database();
        connection_name = db.connectionName();
        db.close();
    }
    QSqlDatabase::removeDatabase(connection_name);
    LOG << "Database connection closed.";
}

const QStringList Database::getTableColumns(OPL::DbTable table_name) const
{
    return m_tableColumns.value(OPL::GLOBALS->getDbTableName(table_name));
}

const QStringList Database::getTableNames() const { return m_tableNames; }

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
    on_database_updated(DbTable::Any);
}

const QString Database::sqliteVersion() const
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT sqlite_version()"));
    query.exec();
    query.next();
    return query.value(0).toString();
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
    // create statement
    const QString table_name = OPL::GLOBALS->getDbTableName(table);
    QString statement        = QLatin1String("INSERT INTO ") + table_name + QLatin1String(" (");
    QString placeholder      = QStringLiteral(") VALUES (");
    for (const auto &column_name : DB->getTableColumns(table)) {
        statement += column_name + ',';
        placeholder.append(QLatin1Char(':') + column_name + QLatin1Char(','));
    }

    statement.chop(1);
    placeholder.chop(1);
    placeholder.append(')');
    statement.append(placeholder);

    // Create query and commit
    QSqlQuery q;
    q.prepare(QStringLiteral("BEGIN EXCLUSIVE TRANSACTION"));
    q.exec();
    for (const auto &entry : json_arr) {
        q.prepare(statement);
        auto object     = entry.toObject();
        const auto keys = object.keys();

        for (const auto &key : keys) {
// use QMetaType for binding null value in QT >= 6
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            object.value(key).isNull()
                ? q.bindValue(key, QVariant(QMetaType(QMetaType::Int)))
                :
#else
            object.value(key).isNull()
                ? q.bindValue(key, QVariant(QVariant::String))
                :
#endif
                q.bindValue(QLatin1Char(':') + key, object.value(key).toVariant());
        }
        q.exec();
    }

    q.prepare(QStringLiteral("COMMIT"));
    if (q.exec())
        return true;
    else
        return false;
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
        LOG << "Error: Database entry not found.";
        return false;
    }

    const QString table_name = OPL::GLOBALS->getDbTableName(row.getTable());

    QString statement =
        QLatin1String("DELETE FROM ") + table_name + QLatin1String(" WHERE ROWID=?");

    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(row.getRowId());

    if (query.exec()) {
        LOG << "Entry removed:";
        LOG << row;
        on_database_updated(row.getTable());
        return true;
    }
    else {
        DEB << "Unable to delete.";
        DEB << "Query: " << statement;
        DEB << "Query Error: " << query.lastError().text();
        m_lastError = query.lastError();
        return false;
    }
}

bool Database::remove(OPL::DbTable table, int row_id)
{
    const QString table_name = OPL::GLOBALS->getDbTableName(table);

    QString statement =
        QLatin1String("DELETE FROM ") + table_name + QLatin1String(" WHERE ROWID=?");

    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(row_id);

    if (query.exec()) {
        DEB << statement;
        DEB << query.lastQuery();
        LOG << "Entry removed: Table " << table_name << ", ROWID " << row_id;
        on_database_updated(table);
        return true;
    }
    else {
        DEB << "Unable to delete.";
        DEB << "Query: " << statement;
        DEB << "Query Error: " << query.lastError().text();
        m_lastError = query.lastError();
        return false;
    }
}

bool Database::removeMany(OPL::DbTable table, const QList<int> &row_id_list)
{
    const QString table_name = OPL::GLOBALS->getDbTableName(table);
    int errorCount           = 0;

    QSqlQuery query;
    query.prepare(QStringLiteral("BEGIN EXCLUSIVE TRANSACTION"));
    query.exec();

    for (const auto row_id : row_id_list) {
        const QString statement =
            QLatin1String("DELETE FROM ") + table_name + QLatin1String(" WHERE ROWID=?");

        query.prepare(statement);
        query.addBindValue(row_id);

        if (!query.exec()) errorCount++;
    }

    if (errorCount == 0) {
        query.prepare(QStringLiteral("COMMIT"));
        if (query.exec()) {
            on_database_updated(table);
            LOG << "Transaction successfull.";
            return true;
        }
        else {
            LOG << "Transaction unsuccessful (Interrupted). Error count: " +
                       QString::number(errorCount);
            DEB << query.lastError().text();
            m_lastError = query.lastError();
            return false;
        }
    }
    else {
        query.prepare(QStringLiteral("ROLLBACK"));
        query.exec();
        LOG << "Transaction unsuccessful (no changes have been made). Error count: " +
                   QString::number(errorCount);
        return false;
    }
}

bool Database::exists(const OPL::Row &row)
{
    const int row_id = row.getRowId();
    if (row_id == 0) return false;

    // QString statement = "SELECT 1 FROM " + OPL::GLOBALS->getDbTableName(row.getTable()) + " WHERE
    // ROWID=? LIMIT 1";

    QString statement = QStringLiteral("SELECT 1 FROM %1 WHERE ROWID=? LIMIT 1")
                            .arg(OPL::GLOBALS->getDbTableName(row.getTable()));
    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(row_id);

    if (!query.exec()) return false;

    return query.next(); // true only if a row exists
}

bool Database::exists(DbTable table, int row_id)
{
    if (row_id == 0) return false;

    QString statement =
        QStringLiteral("SELECT 1 FROM %1 WHERE ROWID=?").arg(OPL::GLOBALS->getDbTableName(table));

    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(row_id);

    if (!query.exec()) return false;

    return query.next(); // true only if a row exists
}

bool Database::update(const OPL::Row &updated_row)
{
    const auto &data          = updated_row.getData();
    const QString quote       = QStringLiteral("\"");
    const QString placeholder = QStringLiteral("\"=?");

    QStringList columns;
    for (const auto &key : data.keys()) {
        columns << quote + key + placeholder;
    }

    QString statement = QString("UPDATE %1 SET %2 WHERE ROWID=?")
                            .arg(updated_row.getTableName(), columns.join(','));

    QSqlQuery query;
    query.prepare(statement);

    for (auto it = data.cbegin(); it != data.cend(); ++it) {
        if (it.value().isNull() || it.value().toString() == QString()) {
            query.addBindValue(QVariant(it.value().metaType()));
        }
        else {
            query.addBindValue(it.value());
        }
    }

    query.addBindValue(updated_row.getRowId());

    DEB << "Statement: " << statement;
    DEB << "Bound values: " << query.boundValues();

    if (!query.exec()) {
        DEB << "Unable to commit." << query.lastError().text();
        m_lastError = query.lastError();
        return false;
    }

    LOG << QString("Entry successfully committed. %1").arg(updated_row.getPosition());
    on_database_updated(updated_row.getTable());
    return true;
}

bool Database::insert(const OPL::Row &new_row)
{
    const auto &data = new_row.getData();
    DEB << data;
    const QString quote       = QStringLiteral("\"");
    const QString placeholder = QStringLiteral("?");
    QStringList columns, placeholders;
    for (auto it = data.cbegin(); it != data.cend(); ++it) {
        columns << (quote + it.key() + quote);
        placeholders << placeholder;
    }

    QString statement = QString("INSERT INTO %1 (%2) VALUES (%3)")
                            .arg(new_row.getTableName(), columns.join(','), placeholders.join(','));

    QSqlQuery query;
    query.prepare(statement);

    for (auto it = data.cbegin(); it != data.cend(); ++it) {
        if (it.value().isNull() || it.value().toString() == QString()) {
            query.addBindValue(QVariant(it.value().metaType()));
        }
        else {
            query.addBindValue(it.value());
        }
    }

    if (!query.exec()) {
        DEB << "Unable to commit." << statement << query.boundValues() << query.lastError().text();
        m_lastError = query.lastError();
        return false;
    }

    LOG << QString("Entry successfully committed. %1").arg(new_row.getPosition());
    on_database_updated(new_row.getTable());
    return true;
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
    auto deleteWhere = [this](const QString &table_name, const QString &column_name,
                              int row_id) -> bool {
        QString query = QStringLiteral("DELETE FROM %1 WHERE %2=?").arg(table_name, column_name);
        QSqlQuery q;
        q.prepare(query);
        q.addBindValue(row_id);

        if (!q.exec()) {
            DEB << "Unable to commit." << query << q.boundValues() << q.lastError().text();
            m_lastError = q.lastError();
            return false;
        }

        return true;
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
    if (!deleteWhere(GLOBALS->getDbTableName(DbTable::FlightSegments), S_COL_FLIGHT_ID,
                     flight_data.flightId())) {
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
    if (!deleteWhere(GLOBALS->getDbTableName(DbTable::MovementEvents), S_COL_EVENT_ID,
                     flight_data.eventId())) {
        return false;
    }
    auto movements = flight_data.movements();
    for (const auto &m : std::as_const(movements)) {
        DEB << "Commiting Movement:" << m;
        commit(m);
    }

    // Approaches
    if (!deleteWhere(GLOBALS->getDbTableName(DbTable::ApproachEvents), S_COL_EVENT_ID,
                     flight_data.eventId())) {
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
    QString statement = QLatin1String("SELECT * FROM ") + OPL::GLOBALS->getDbTableName(table) +
                        QLatin1String(" WHERE ROWID=?");
    QSqlQuery q;
    q.prepare(statement);
    q.addBindValue(row_id);
    q.setForwardOnly(true);

    if (!q.exec()) {
        DEB << "SQL error: " << q.lastError().text();
        DEB << "Statement: " << q.lastQuery();
        m_lastError = q.lastError();
        return {}; // return invalid Row
    }

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
    QString statement = QLatin1String("SELECT * FROM ") + OPL::GLOBALS->getDbTableName(table) +
                        QLatin1String(" WHERE ") + filter_column + QLatin1String(" = ?");
    QSqlQuery q;
    q.prepare(statement);
    q.addBindValue(row_id);
    q.setForwardOnly(true);

    if (!q.exec()) {
        DEB << "SQL error: " << q.lastError().text();
        DEB << "Statement: " << q.lastQuery();
        m_lastError = q.lastError();
        return {}; // return invalid Row
    }

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

QList<FlightSegmentEntry> Database::getFlightSegments(int flight_id)
{
    const QString statement = QStringLiteral("SELECT * FROM flight_segments WHERE flight_id = ?");

    QSqlQuery q;
    q.prepare(statement);
    q.addBindValue(flight_id);
    q.setForwardOnly(true);

    if (!q.exec()) {
        DEB << "SQL error: " << q.lastError().text();
        DEB << "Statement: " << q.lastQuery();
        m_lastError = q.lastError();
        return {}; // return invalid Row
    }

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
            DEB << "Added Row: " << entry_data;
        }
    }

    if (rows.isEmpty()) {
        return {};
    }

    QList<FlightSegmentEntry> result;
    for (const auto &rowData : rows) {
        result.append(FlightSegmentEntry(flight_id, rowData));
    }
    return result;
}

QList<MovementEntry> Database::getMovementEntries(int event_id)
{
    const QString statement = QStringLiteral("SELECT * FROM movement_events WHERE event_id = ?");

    QSqlQuery q;
    q.prepare(statement);
    q.addBindValue(event_id);
    q.setForwardOnly(true);

    if (!q.exec()) {
        DEB << "SQL error: " << q.lastError().text();
        DEB << "Statement: " << q.lastQuery();
        m_lastError = q.lastError();
        return {}; // return invalid Row
    }

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
            DEB << "Added Row: " << entry_data;
        }
    }

    if (rows.isEmpty()) {
        return {};
    }

    QList<MovementEntry> result;
    for (const auto &rowData : rows) {
        result.append(MovementEntry(event_id, rowData));
    }
    return result;
}

QList<AirportCodeEntry> Database::getAirportCodeEntries(int airport_id)
{
    const QString statement = QStringLiteral("SELECT * FROM airport_codes WHERE airport_id = ?");

    QSqlQuery q;
    q.prepare(statement);
    q.addBindValue(airport_id);
    q.setForwardOnly(true);

    if (!q.exec()) {
        DEB << "SQL error: " << q.lastError().text();
        DEB << "Statement: " << q.lastQuery();
        m_lastError = q.lastError();
        return {}; // return invalid Row
    }

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
            DEB << "Added Row: " << entry_data;
        }
    }

    if (rows.isEmpty()) {
        return {};
    }

    QList<AirportCodeEntry> result;
    for (const auto &rowData : rows) {
        result.append(AirportCodeEntry(airport_id, rowData));
    }
    return result;
}

int Database::getLastEntry(OPL::DbTable table)
{
    QString statement =
        QLatin1String("SELECT MAX(ROWID) FROM ") + OPL::GLOBALS->getDbTableName(table);

    auto query = QSqlQuery(statement);
    if (query.first()) {
        return query.value(0).toInt();
    }
    else {
        LOG << "No entry found. (Database empty?)" << query.lastError().text();
        return 0;
    }
}

void Database::on_database_updated(DbTable table)
{
    LOG << "Emitting database update for table: " << OPL::GLOBALS->getDbTableName(table);
    emit dataBaseUpdated(table);
}

QList<int> Database::getForeignKeyConstraints(int foreign_row_id, OPL::DbTable table)
{
    QString statement;

    switch (table) {
    case OPL::DbTable::Pilots:
        statement = QStringLiteral("SELECT ROWID FROM flights WHERE pic=?");
        break;
    case OPL::DbTable::AircraftTails:
        statement = QStringLiteral("SELECT ROWID FROM flights WHERE tail_id=?");
        break;
    case OPL::DbTable::AircraftTypes:
        statement = QStringLiteral("SELECT ROWID FROM aircraft_tails WHERE aircraft_type_id=?");
        break;
    default:
        DEB << "Not a valid target for this function.";
        return QList<int>();
        break;
    }

    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(foreign_row_id);
    query.exec();

    if (!query.isActive()) {
        m_lastError = query.lastError();
        DEB << "Error";
        DEB << statement;
        DEB << query.lastError().text();
        return QList<int>();
    }

    QList<int> row_ids;
    while (query.next()) {
        row_ids.append(query.value(0).toInt());
    }
    return row_ids;
}

QVector<RowData_T> Database::getTable(OPL::DbTable table)
{
    const QString query_str = QStringLiteral("SELECT * FROM ") + GLOBALS->getDbTableName(table);

    QSqlQuery q;
    q.prepare(query_str);
    q.setForwardOnly(true);

    if (!q.exec()) {
        LOG << "SQL error: " << q.lastError().text();
        LOG << "Statement: " << query_str;
        m_lastError = q.lastError();
        return {};
    }

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

OPL::FlightData Database::getFlightData(int event_id)
{
    const auto log_data  = getRowData(OPL::DbTable::LogEvents, event_id);
    const auto log_entry = OPL::LogEntry(event_id, log_data);

    const auto flight_data = getRowData(DbTable::Flights, QStringLiteral("event_id"), event_id);

    const auto flight_entry =
        OPL::FlightLogEntry(flight_data.value(QStringLiteral("flight_id")).toInt(), flight_data);

    const auto movements = getMovementEntries(event_id);

    const auto segments = getFlightSegments(flight_entry.getRowId());

    // collect approach data

    return {log_entry, flight_entry, segments, movements};
}
} // namespace OPL
