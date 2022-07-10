/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#include "src/functions/alog.h"
#include "src/classes/astandardpaths.h"
#include "src/opl.h"
#include "src/functions/alog.h"
#include "src/classes/ajson.h"

const int ADatabase::minimumDatabaseRevision = 0;

const QStringList ADatabase::userTableNames = {
    QStringLiteral("flights"),
    QStringLiteral("pilots"),
    QStringLiteral("tails")
};
const QStringList ADatabase::templateTableNames = {
    QStringLiteral("aircraft"),
    QStringLiteral("airports"),
    QStringLiteral("currencies"),
    QStringLiteral("changelog")
};

ADatabase* ADatabase::self = nullptr;

ADatabase::ADatabase()
    : databaseFile(QFileInfo(AStandardPaths::directory(AStandardPaths::Database).
                             absoluteFilePath(QStringLiteral("logbook.db"))))
{}

int ADatabase::dbRevision() const
{
    return databaseRevision;
}

int ADatabase::checkDbVersion() const
{
    QSqlQuery query(QStringLiteral("SELECT COUNT(*) FROM changelog"));
    query.next();
    return query.value(0).toInt();
}

int ADatabase::getMinimumDatabaseRevision()
{
    return minimumDatabaseRevision;
}

const QStringList &ADatabase::getTemplateTableNames() const
{
    return templateTableNames;
}

const QStringList& ADatabase::getUserTableNames() const
{
    return userTableNames;
}

UserDataState ADatabase::getUserDataState()
{
    QSqlQuery q;
    q.prepare(QStringLiteral("SELECT COUNT (*) FROM tails"));
    q.exec();
    q.first();
    int tails = q.value(0).toInt();

    q.prepare(QStringLiteral("SELECT COUNT (*) FROM pilots"));
    q.exec();
    q.first();
    int pilots = q.value(0).toInt();

    return UserDataState(tails, pilots);
}

bool ADatabase::resetUserData()
{
    QSqlQuery query;
    for (const auto& table : userTableNames) {
        query.prepare(QLatin1String("DELETE FROM ") + table);
        if (!query.exec()) {
            DEB << "Error: " << query.lastError().text();
            return false;
        }
    }
    return true;
}

const ColumnNames_T ADatabase::getTableColumns(TableName_T table_name) const
{
    return tableColumns.value(table_name);
}

const TableNames_T ADatabase::getTableNames() const
{
    return tableNames;
}

void ADatabase::updateLayout()
{
    auto db = ADatabase::database();
    tableNames = db.tables();

    tableColumns.clear();
    for (const auto &table_name : qAsConst(tableNames)) {
        ColumnNames_T table_columns;
        QSqlRecord fields = db.record(table_name);
        for (int i = 0; i < fields.count(); i++) {
            table_columns.append(fields.field(i).name());
        }
        tableColumns.insert(table_name, table_columns);
    }
    emit dataBaseUpdated();
}

ADatabase* ADatabase::instance()
{
    if(!self)
        self = new ADatabase();

    return self;
}

bool ADatabase::createSchema()
{
    // Read Database layout from sql file
    QFile f(OPL::Assets::DATABASE_SCHEMA);
    f.open(QIODevice::ReadOnly);
    QByteArray filedata = f.readAll();
    // create individual queries for each table/view
    auto list = filedata.split(';');

    // Create Tables
    QSqlQuery q;
    QVector<QSqlError> errors;
    for (const auto &query_string : list) {
        q.prepare(query_string);
        if (!q.exec()) {
            errors.append(q.lastError());
            LOG << "Unable to execute query: ";
            LOG << q.lastQuery();
            LOG << q.lastError();
        }
    }
    updateLayout();

    if (errors.isEmpty()) {
        LOG << "Database succesfully created.";
        return true;
    } else {
        LOG << "Database creation has failed. The following error(s) have ocurred: ";
        for (const auto &error : qAsConst(errors)) {
            LOG << error.type() << error.text();
        }
        return false;
    }
}

bool ADatabase::importTemplateData(bool use_local_ressources)
{
    for (const auto& table_name : templateTableNames) {

        //clear table
        QSqlQuery q;
        q.prepare(QLatin1String("DELETE FROM ") + table_name);
        if (!q.exec()) {
            LOG << "Error clearing tables: " << q.lastError().text();
            return false;
        }

        //Prepare data
        QJsonArray data_to_commit;
        QString error_message("Error importing data ");

        if (use_local_ressources) {
            data_to_commit = AJson::readFileToDoc(QLatin1String(":database/templates/")
                                      + table_name + QLatin1String(".json")).array();
            error_message.append(QLatin1String(" (ressource) "));
        } else {
            data_to_commit = AJson::readFileToDoc(AStandardPaths::directory(
                                          AStandardPaths::Templates).absoluteFilePath(
                                          table_name + QLatin1String(".json"))).array();
            error_message.append(QLatin1String(" (downloaded) "));
        }

        // commit Data from Array
        if (!commit(data_to_commit, table_name)) {
            LOG << error_message;
            return false;
        }
    } // for table_name
    return true;
}

const QString ADatabase::sqliteVersion() const
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT sqlite_version()"));
    query.exec();
    query.next();
    return query.value(0).toString();
}

bool ADatabase::connect()
{
    if (!QSqlDatabase::isDriverAvailable(SQLITE_DRIVER)) {
        LOG << "Error: No SQLITE Driver availabe.";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(SQLITE_DRIVER);
    db.setDatabaseName(databaseFile.absoluteFilePath());

    if (!db.open()) {
        LOG << QString("Unable to establish database connection.<br>The following error has ocurred:<br><br>%1")
               .arg(db.lastError().databaseText());
        lastError = db.lastError();
        return false;
    }


    LOG << "Database connection established.";
    // Enable foreign key restrictions
    QSqlQuery query;
    query.prepare(QStringLiteral("PRAGMA foreign_keys = ON;"));
    query.exec();
    updateLayout();
    databaseRevision = checkDbVersion();
    return true;
}

void ADatabase::disconnect()
{
    auto db = ADatabase::database();
    db.close();
    db.removeDatabase(db.connectionName());
    LOG << "Database connection closed.";
}

QSqlDatabase ADatabase::database()
{
    return QSqlDatabase::database(QStringLiteral("qt_sql_default_connection"));
}

//bool ADatabase::commit(const AEntry &entry)
//{
//    if (exists(entry)) {
//        return update(entry);
//    } else {
//        return insert(entry);
//    }
//}

bool ADatabase::commit(const OPL::Row &row)
{
    if (!row.isValid())
        return false;

    if (exists(row))
        return update(row);
    else
        return insert(row);
}

bool ADatabase::commit(const QJsonArray &json_arr, const QString &table_name)
{
    // create statement
    QString statement = QLatin1String("INSERT INTO ") + table_name + QLatin1String(" (");
    QString placeholder = QStringLiteral(") VALUES (");
    for (const auto &column_name : aDB->getTableColumns(table_name)) {
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
        auto object = entry.toObject();
        const auto keys = object.keys();
        for (const auto &key : keys){
            object.value(key).isNull() ? q.bindValue(key, QVariant(QVariant::String)) : // refactor to use QMetaType with Qt6
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

//bool ADatabase::remove(const AEntry &entry)
//{
//    if (!exists(entry)) {
//        LOG << "Error: Database entry not found.";
//        return false;
//    }
//
//    QString statement = QLatin1String("DELETE FROM ") + entry.getPosition().tableName
//            + QLatin1String(" WHERE ROWID=?");
//
//    QSqlQuery query;
//    query.prepare(statement);
//    query.addBindValue(entry.getPosition().rowId);
//
//    if (query.exec())
//    {
//        LOG << "Entry " << entry.getPosition() << " removed.";
//        emit dataBaseUpdated();
//        return true;
//    } else {
//        DEB << "Unable to delete.";
//        DEB << "Query: " << statement;
//        DEB << "Query Error: " << query.lastError().text();
//        lastError = query.lastError();
//        return false;
//    }
//}

bool ADatabase::remove(const OPL::Row &row)
{
    if (!exists(row)) {
        LOG << "Error: Database entry not found.";
        return false;
    }

    QString statement = QLatin1String("DELETE FROM ") + OPL::GLOBALS->getDbTableName(row.getTableName())
            + QLatin1String(" WHERE ROWID=?");

    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(row.getRowId());

    if (query.exec())
    {
        LOG << "Entry removed:";
        LOG << row;
        emit dataBaseUpdated();
        return true;
    } else {
        DEB << "Unable to delete.";
        DEB << "Query: " << statement;
        DEB << "Query Error: " << query.lastError().text();
        lastError = query.lastError();
        return false;
    }
}

bool ADatabase::removeMany(const QList<DataPosition> &position_list)
{
    int errorCount = 0;
    QSqlQuery query;
    query.prepare(QStringLiteral("BEGIN EXCLUSIVE TRANSACTION"));
    query.exec();

    for (const auto& data_position : position_list) {
        if (!exists(data_position)) {
            errorCount++;
        }
        QString statement = QLatin1String("DELETE FROM ") + data_position.tableName +
                QLatin1String(" WHERE ROWID=?");

        query.prepare(statement);
        query.addBindValue(data_position.rowId);

        if (!query.exec())
            errorCount++;
    }

    if (errorCount == 0) {
        query.prepare(QStringLiteral("COMMIT"));
        if(query.exec()) {
            emit dataBaseUpdated();
            LOG << "Transaction successfull.";
            return true;
        } else {
            LOG << "Transaction unsuccessful (Interrupted). Error count: "
                        + QString::number(errorCount);
            DEB << query.lastError().text();
            lastError = query.lastError();
            return false;
        }
    } else {
        query.prepare(QStringLiteral("ROLLBACK"));
        query.exec();
        LOG << "Transaction unsuccessful (no changes have been made). Error count: "
                    + QString::number(errorCount);
        return false;
    }
}

bool ADatabase::exists(const DataPosition &data_position)
{
    if(data_position.rowId == 0)
        return false;

    //Check database for row id
    QString statement = QLatin1String("SELECT COUNT(*) FROM ") + data_position.tableName
            + QLatin1String(" WHERE ROWID=?");
    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(data_position.rowId);
    query.setForwardOnly(true);
    query.exec();
    //this returns either 1 or 0 since row ids are unique
    if (!query.isActive()) {
        lastError = query.lastError();
        LOG << "Query Error: " << query.lastError().text() << statement;
    }
    query.next();
    int rowId = query.value(0).toInt();
    if (rowId) {
        return true;
    } else {
        LOG << "No entry exists at DataPosition: " << data_position.tableName << data_position.rowId;
        return false;
    }
}

bool ADatabase::exists(const OPL::Row &row)
{
    if (row.getRowId() == 0)
        return false;

    //Check database for row id
    QString statement = QLatin1String("SELECT COUNT(*) FROM ") + OPL::GLOBALS->getDbTableName(row.getTableName())
            + QLatin1String(" WHERE ROWID=?");
    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(row.getRowId());
    query.setForwardOnly(true);
    query.exec();
    //this returns either 1 or 0 since row ids are unique
    if (!query.isActive()) {
        lastError = query.lastError();
        DEB << "Query Error: " << query.lastError().text() << statement;
        return false;
    }
    query.next();
    int rowId = query.value(0).toInt();
    if (rowId) {
        return true;
    } else {
        LOG << "Database entry not found.";
        return false;
    }
}

bool ADatabase::clear()
{
    QSqlQuery q;

    for (const auto &table_name : userTableNames) {
        q.prepare(QLatin1String("DELETE FROM ") + table_name);
        if (!q.exec()) {
            DEB << "Error: " << q.lastError().text();
            lastError = q.lastError();
            return false;
        }
    }
    return true;
}

bool ADatabase::update(const OPL::Row &updated_row)
{
    QString statement = QLatin1String("UPDATE ") + OPL::GLOBALS->getDbTableName(updated_row.getTableName()) + QLatin1String(" SET ");
    for (auto i = updated_row.getData().constBegin(); i != updated_row.getData().constEnd(); ++i) {
        statement.append(i.key() + "=?,");
    }
    statement.chop(1);
    statement.append(QLatin1String(" WHERE ROWID=?"));
    QSqlQuery query;
    query.prepare(statement);
    DEB << "Statement: " << statement;
    for (auto i = updated_row.getData().constBegin(); i != updated_row.getData().constEnd(); ++i) {
        if (i.value() == QVariant(QString()) || i.value() == 0) {
            query.addBindValue(QVariant(QVariant::String));
        } else {
            query.addBindValue(i.value());
        }
    }
    query.addBindValue(updated_row.getRowId());
    DEB << "Bound values: " << query.boundValues();

    if (query.exec())
    {
        DEB << "Entry successfully committed.";
        emit dataBaseUpdated();
        return true;
    } else {
        DEB << "Unable to commit.";
        DEB << "Query: " << statement;
        DEB << "Query Error: " << query.lastError().text();
        lastError = query.lastError();
        return false;
    }
}

bool ADatabase::insert(const OPL::Row &new_row)
{
    QString statement = QLatin1String("INSERT INTO ") + OPL::GLOBALS->getDbTableName(new_row.getTableName()) + QLatin1String(" (");
    QHash<QString, QVariant>::const_iterator i;
    for (i = new_row.getData().constBegin(); i != new_row.getData().constEnd(); ++i) {
        statement.append(i.key() + QLatin1Char(','));
    }
    statement.chop(1);
    statement += QLatin1String(") VALUES (");

    for (int i=0; i < new_row.getData().size(); ++i) {
        statement += QLatin1String("?,");
    }
    statement.chop(1);
    statement += QLatin1Char(')');

    QSqlQuery query;
    query.prepare(statement);

    for (i = new_row.getData().constBegin(); i != new_row.getData().constEnd(); ++i) {
        if (i.value() == QVariant(QString()) || i.value() == 0) {
            query.addBindValue(QVariant(QVariant::String));
        } else {
            query.addBindValue(i.value());
        }
    }

    //check result.
    if (query.exec())
    {
        DEB << "Entry successfully committed.";
        emit dataBaseUpdated();
        return true;
    } else {
        DEB << "Unable to commit.";
        DEB << "Query: " << statement;
        DEB << "Query Error: " << query.lastError().text();
        lastError = query.lastError();
        return false;
    }
}

RowData_T ADatabase::getEntryData(const DataPosition &data_position)
{
    // check table exists
    if (!getTableNames().contains(data_position.tableName)) {
        DEB << data_position.tableName << " not a table in the database. Unable to retreive Entry data.";
        return RowData_T();
    }

    //Check Database for rowId
    QString statement = QLatin1String("SELECT COUNT(*) FROM ") + data_position.tableName
                      + QLatin1String(" WHERE ROWID=?");
    QSqlQuery check_query;
    check_query.prepare(statement);
    check_query.addBindValue(data_position.rowId);
    check_query.setForwardOnly(true);

    if (!check_query.exec()) {
        DEB << "SQL error: " << check_query.lastError().text();
        DEB << "Statement: " << statement;
        lastError = check_query.lastError();
        return RowData_T();
    }
    check_query.next();
    if (check_query.value(0).toInt() == 0) {
        LOG << "No Entry found for row id: " << data_position.rowId;
        return RowData_T();
    }

    // Retreive TableData
    statement = QLatin1String("SELECT * FROM ") + data_position.tableName
              + QLatin1String(" WHERE ROWID=?");

    QSqlQuery select_query;
    select_query.prepare(statement);
    select_query.addBindValue(data_position.rowId);
    select_query.setForwardOnly(true);


    if (!select_query.exec()) {
        DEB << "SQL error: " << select_query.lastError().text();
        DEB << "Statement: " << select_query.lastQuery();
        lastError = select_query.lastError();
        return {};
    }

    RowData_T entry_data;
    if(select_query.next()) { // retreive records
        auto r = select_query.record();
        for (int i = 0; i < r.count(); i++){
            if(!r.value(i).isNull()) {
                entry_data.insert(r.fieldName(i), r.value(i));
            }
        }
    }
    return entry_data;
}

OPL::Row ADatabase::getRow(const OPL::DbTable table, const int row_id)
{
    QString statement = QLatin1String("SELECT * FROM ") + OPL::GLOBALS->getDbTableName(table)
            + QLatin1String(" WHERE ROWID=?");
    QSqlQuery q;
    q.prepare(statement);
    q.addBindValue(row_id);
    q.setForwardOnly(true);

    if (!q.exec()) {
        DEB << "SQL error: " << q.lastError().text();
        DEB << "Statement: " << q.lastQuery();
        lastError = q.lastError();
        return {}; // return invalid Row
    }

    RowData_T entry_data;
    if(q.next()) {
        auto r = q.record(); // retreive record
        if (r.count() == 0)  // row is empty
            return {};

        for (int i = 0; i < r.count(); i++){ // iterate through fields to get key:value map
            if(!r.value(i).isNull()) {
                entry_data.insert(r.fieldName(i), r.value(i));
            }
        }
    }

    return OPL::Row(table, row_id, entry_data);
}

RowData_T ADatabase::getRowData(const OPL::DbTable table, const int row_id)
{
    QString statement = QLatin1String("SELECT * FROM ") + OPL::GLOBALS->getDbTableName(table)
            + QLatin1String(" WHERE ROWID=?");
    QSqlQuery q;
    q.prepare(statement);
    q.addBindValue(row_id);
    q.setForwardOnly(true);

    if (!q.exec()) {
        DEB << "SQL error: " << q.lastError().text();
        DEB << "Statement: " << q.lastQuery();
        lastError = q.lastError();
        return {}; // return invalid Row
    }

    RowData_T entry_data;
    if(q.next()) {
        auto r = q.record(); // retreive record
        if (r.count() == 0)  // row is empty
            return {};

        for (int i = 0; i < r.count(); i++){ // iterate through fields to get key:value map
            if(!r.value(i).isNull()) {
                entry_data.insert(r.fieldName(i), r.value(i));
            }
        }
    }

    return entry_data;
}

const QStringList ADatabase::getCompletionList(ADatabaseTarget target)
{
    QString statement;

    switch (target) {
    case ADatabaseTarget::pilots:
        statement.append(QStringLiteral("SELECT lastname||', '||firstname FROM pilots"));
        break;
    case ADatabaseTarget::aircraft:
        statement.append(QStringLiteral("SELECT make||' '||model FROM aircraft WHERE model IS NOT NULL AND variant IS NULL "
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
        DEB << "Not a valid completer target for this function.";
        return QStringList();
    }

    QSqlQuery query;
    query.prepare(statement);
    query.setForwardOnly(true);
    query.exec();

    if(!query.isActive()) {
        lastError = query.lastError();
        return QStringList();
    }

    QStringList completer_list;
    while (query.next())
        completer_list.append(query.value(0).toString());

    completer_list.sort();
    completer_list.removeAll(QString());
    completer_list.removeDuplicates();

    return completer_list;
}

const QHash<RowId_T, QString> ADatabase::getIdMap(ADatabaseTarget target)
{
    QString statement;

    switch (target) {
    case ADatabaseTarget::pilots:
        statement.append(QStringLiteral("SELECT ROWID, lastname||', '||firstname FROM pilots"));
        break;
    case ADatabaseTarget::aircraft:
        statement.append(QStringLiteral("SELECT ROWID, make||' '||model FROM aircraft WHERE model IS NOT NULL AND variant IS NULL "
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
        DEB << "Not a valid completer target for this function.";
        return {};
    }

    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare(statement);
    query.exec();
    //auto query = QSqlQuery(statement);
    if (!query.isActive()) {
        DEB << "No result found. Check Query and Error.";
        DEB << "Query: " << statement;
        DEB << "Error: " << query.lastError().text();
        lastError = query.lastError();
        return {};
    }
    auto id_map = QHash<RowId_T, QString>();
    while (query.next())
        id_map.insert(query.value(0).toInt(), query.value(1).toString());
    return id_map;
}

RowId_T ADatabase::getLastEntry(ADatabaseTable table)
{
    QString statement = QLatin1String("SELECT MAX(ROWID) FROM ");

    switch (table) {
    case ADatabaseTable::pilots:
        statement.append(OPL::Db::TABLE_PILOTS);
        break;
    case ADatabaseTable::aircraft:
        statement.append(OPL::Db::TABLE_AIRCRAFT);
        break;
    case ADatabaseTable::tails:
        statement.append(OPL::Db::TABLE_TAILS);
        break;
    default:
        DEB << "Not a valid completer target for this function.";
        return 0;
    }
    auto query = QSqlQuery(statement);
    if (query.first()) {
        return query.value(0).toInt();
    } else {
        LOG << "No entry found. (Database empty?)" << query.lastError().text();
        return 0;
    }
}

QList<RowId_T> ADatabase::getForeignKeyConstraints(RowId_T foreign_row_id, ADatabaseTable target)
{
    QString statement = QLatin1String("SELECT ROWID FROM flights WHERE ");

    switch (target) {
    case ADatabaseTable::pilots:
        statement.append(QLatin1String("pic=?"));
        break;
    case ADatabaseTable::tails:
        statement.append(QLatin1String("acft=?"));
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
        lastError = query.lastError();
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

QVector<QVariant> ADatabase::customQuery(QString statement, int return_values)
{
    QSqlQuery query(statement);
    if(!query.exec()) {
        lastError = query.lastError();
        DEB << "Query Error: " << lastError.text();
        return {};
    }

    if (!query.first()) {
        LOG << "No result found. Check Query and Error.";
        DEB << "Error: " << query.lastError().text();
        DEB << "Statement: " << statement;
        return QVector<QVariant>();
    } else {
        query.first();
        query.previous();
        QVector<QVariant> result;
        while (query.next()) {
            for (int i = 0; i < return_values ; i++) {
                result.append(query.value(i));
            }
        }
        lastError = QString();
        return result;
    }
}

QMap<ADatabaseSummaryKey, QString> ADatabase::databaseSummary(const QString &db_path)
{
    const QString connection_name = QStringLiteral("summary_connection");
    QMap<ADatabaseSummaryKey, QString> return_values;
    { // scope for a temporary database connection, ensures proper cleanup when removeDatabase() is called.
        //DEB << "Adding temporary connection to database:" << db_path;
        QSqlDatabase temp_database = QSqlDatabase::addDatabase(SQLITE_DRIVER, connection_name); // Don't use default connection
        temp_database.setDatabaseName(db_path);
        if (!temp_database.open())
            return {};

        QSqlQuery query(temp_database); // Query object using the temporary connection
        ADatabaseSummaryKey key;  // Used among the queries for verbosity... and sanity

        const QVector<QPair<ADatabaseSummaryKey, QString>> key_table_pairs = {
            {ADatabaseSummaryKey::total_flights, QStringLiteral("flights")},
            {ADatabaseSummaryKey::total_tails, QStringLiteral("tails")},
            {ADatabaseSummaryKey::total_pilots, QStringLiteral("pilots")}
        };
        // retreive amount of flights, tails and pilots
        for (const auto & pair : key_table_pairs) {
            query.prepare(QLatin1String("SELECT COUNT (*) FROM ") + pair.second);
            query.exec();
            key = pair.first;
            if (query.first()){
                return_values[key] = query.value(0).toString();
            }
            else{
                return_values[key] = QString();
            }
        }
        // retreive date of last flight
        query.prepare(QStringLiteral("SELECT MAX(doft) FROM flights"));
        query.exec();
        key = ADatabaseSummaryKey::last_flight;
        if (query.first()){
            return_values[key] = query.value(0).toString();
        }
        else {
            return_values[key] = QString();
        }
        // retreive total flight time as a string "hh:mm"
        query.prepare(QStringLiteral("SELECT "
                                     "printf(\"%02d\",CAST(SUM(tblk) AS INT)/60)"
                                     "||':'||"
                                     "printf(\"%02d\",CAST(SUM(tblk) AS INT)%60) FROM flights"));
        key = ADatabaseSummaryKey::total_time;
        query.exec();
        if (query.first()){
            return_values[key] = query.value(0).toString();
        }
        else {
            return_values[key] = QString();
        }
    }

    QSqlDatabase::removeDatabase(connection_name); // cleanly removes temp connection without leaks since query+db are out of scope

    return return_values;
}

const QString ADatabase::databaseSummaryString(const QString &db_path)
{
    auto summary_map = databaseSummary(db_path);
    QString out = QLatin1String("<table>");
    out.append(tr("<tr><td>Total Time: </td><td>%1</td>").arg(summary_map[ADatabaseSummaryKey::total_time]));
    out.append(tr("<tr><td>Last Flight: </td><td>%1</td>").arg(summary_map[ADatabaseSummaryKey::last_flight]));
    out.append(tr("<tr><td>Number of flights: </td><td>%1</td>").arg(summary_map[ADatabaseSummaryKey::total_flights]));
    out.append(tr("<tr><td>Number of aircraft: </td><td>%1</td>").arg(summary_map[ADatabaseSummaryKey::total_tails]));
    out.append(tr("<tr><td>Number of Pilots: </td><td>%1</td>").arg(summary_map[ADatabaseSummaryKey::total_pilots]));
    out.append("</table>");

    return out;
}

/*!
 * \brief ADatabase::createBackup copies the currently used database to an external backup location provided by the user
 * \param dest_file This is the full path and filename of where the backup will be created, e.g. 'home/Sully/myBackups/backupFromOpl.db'
 */
bool ADatabase::createBackup(const QString& dest_file)
{
    LOG << "Backing up current database to: " << dest_file;
    ADatabase::disconnect();
    QFile db_file(databaseFile.absoluteFilePath());
    //DEB << "File to Overwrite:" << db_file;

    if (!db_file.copy(dest_file)) {
        LOG << "Unable to backup old database:" << db_file.errorString();
        return false;
    }

    LOG << "Backed up old database as:" << dest_file;
    ADatabase::connect();
    emit connectionReset();
    return true;
}

QVector<RowData_T> ADatabase::getTable(ADatabaseTable table_name)
{
    auto query_str = QStringLiteral("SELECT * FROM ");
    switch (table_name) {
    case ADatabaseTable::pilots:
        query_str.append(OPL::Db::TABLE_PILOTS);
        break;
    case ADatabaseTable::tails:
        query_str.append(OPL::Db::TABLE_TAILS);
        break;
    case ADatabaseTable::flights:
        query_str.append(OPL::Db::TABLE_FLIGHTS);
        break;
    case ADatabaseTable::currencies:
        query_str.append(OPL::Db::TABLE_CURRENCIES);
    default:
        break;
    }

    QSqlQuery q;
    q.prepare(query_str);
    q.setForwardOnly(true);

    if (!q.exec()) {
        LOG << "SQL error: " << q.lastError().text();
        LOG << "Statement: " << query_str;
        lastError = q.lastError();
        return {};
    }

    QVector<RowData_T> entry_data;
    while(q.next()) { // iterate through records
        auto r = q.record();
        //DEB << r;
        RowData_T row;
        for (int i = 0; i < r.count(); i++){
            if(!r.value(i).isNull()) {
                row.insert(r.fieldName(i), r.value(i));
            }
        }
        entry_data.append(row);
    }
    return entry_data;
}

/*!
 * \brief ADatabase::restoreBackup restores the database from a given backup file and replaces the currently active database.
 * \param backup_file This is the full path and filename of the backup, e.g. 'home/Sully/myBackups/backupFromOpl.db'
 */
bool ADatabase::restoreBackup(const QString& backup_file)
{
    LOG << "Restoring backup from file:" << backup_file;

    QString default_loc = databaseFile.absoluteFilePath();

    ADatabase::disconnect();
    QFile backup(backup_file);
    QFile current_db(default_loc);

    if (!current_db.rename(default_loc + QLatin1String(".tmp"))) { // move previously used db out of the way
        LOG << current_db.errorString() << "Unable to remove current db file";
        return false;
    }

    if (!backup.copy(default_loc))
    {
        LOG << backup.errorString() << "Could not copy" << backup.fileName() << "to" << databaseFile;
        // try to restore previously used db
        current_db.rename(default_loc);
        return false;
    }

    // backup has been restored, clean up the previously moved file
    current_db.remove();
    LOG << "Backup successfully restored!";
    ADatabase::connect();
    emit connectionReset();
    return true;
}

