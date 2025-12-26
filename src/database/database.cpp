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
#include "src/opl.h"
#include "src/classes/jsonhelper.h"

namespace OPL {

bool Database::connect()
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


    LOG << "Database connection established: " + databaseFile.absoluteFilePath();
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
    auto db = Database::database();
    connection_name = db.connectionName();
    db.close();
    }
    QSqlDatabase::removeDatabase(connection_name);
    LOG << "Database connection closed.";
}

const QList<OPL::DbTable> &Database::getTemplateTables() const
{
    return TEMPLATE_TABLES;
}

const QList<OPL::DbTable> &Database::getUserTables() const
{
    return USER_TABLES;
}

const QStringList Database::getTableColumns(OPL::DbTable table_name) const
{
    return tableColumns.value(OPL::GLOBALS->getDbTableName(table_name));
}

const QStringList Database::getTableNames() const
{
    return tableNames;
}

void Database::updateLayout()
{
    auto db = Database::database();
    tableNames = db.tables();

    tableColumns.clear();
    for (const auto &table_name : std::as_const(tableNames)) {
        QStringList table_columns;
        QSqlRecord fields = db.record(table_name);
        for (int i = 0; i < fields.count(); i++) {
            table_columns.append(fields.field(i).name());
        }
        tableColumns.insert(table_name, table_columns);
    }
    emit dataBaseUpdated(DbTable::Any);
}


const QString Database::sqliteVersion() const
{
    QSqlQuery query;
    query.prepare(QStringLiteral("SELECT sqlite_version()"));
    query.exec();
    query.next();
    return query.value(0).toString();
}

QSqlDatabase Database::database()
{
    return QSqlDatabase::database(QStringLiteral("qt_sql_default_connection"));
}

bool Database::commit(const OPL::Row &row)
{
    if (!row.isValid())
        return false;

    if (exists(row))
        return update(row);
    else
        return insert(row);
}

bool Database::commit(const QJsonArray &json_arr, const OPL::DbTable table)
{
    // create statement
    const QString table_name = OPL::GLOBALS->getDbTableName(table);
    QString statement = QLatin1String("INSERT INTO ") + table_name + QLatin1String(" (");
    QString placeholder = QStringLiteral(") VALUES (");
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
        auto object = entry.toObject();
        const auto keys = object.keys();

        for (const auto &key : keys){
//use QMetaType for binding null value in QT >= 6
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            object.value(key).isNull() ? q.bindValue(key, QVariant(QMetaType(QMetaType::Int))) :
#else
            object.value(key).isNull() ? q.bindValue(key, QVariant(QVariant::String)) :
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

bool Database::remove(const OPL::Row &row)
{
    if (!exists(row)) {
        LOG << "Error: Database entry not found.";
        return false;
    }

    const QString table_name = OPL::GLOBALS->getDbTableName(row.getTable());

    QString statement = QLatin1String("DELETE FROM ") + table_name
            + QLatin1String(" WHERE ROWID=?");

    QSqlQuery query;
    query.prepare(statement);
    query.addBindValue(row.getRowId());

    if (query.exec())
    {
        LOG << "Entry removed:";
        LOG << row;
        emit dataBaseUpdated(row.getTable());
        return true;
    } else {
        DEB << "Unable to delete.";
        DEB << "Query: " << statement;
        DEB << "Query Error: " << query.lastError().text();
        lastError = query.lastError();
        return false;
    }
}

bool Database::removeMany(OPL::DbTable table, const QList<int> &row_id_list)
{
    const QString table_name = OPL::GLOBALS->getDbTableName(table);
    int errorCount = 0;

    QSqlQuery query;
    query.prepare(QStringLiteral("BEGIN EXCLUSIVE TRANSACTION"));
    query.exec();

    for (const auto row_id : row_id_list) {
        const QString statement = QLatin1String("DELETE FROM ") + table_name +
                QLatin1String(" WHERE ROWID=?");

        query.prepare(statement);
        query.addBindValue(row_id);

        if (!query.exec())
            errorCount++;
    }

    if (errorCount == 0) {
        query.prepare(QStringLiteral("COMMIT"));
        if(query.exec()) {
            emit dataBaseUpdated(table);
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

bool Database::exists(const OPL::Row &row)
{
    if (row.getRowId() == 0)
        return false;

    //Check database for row id
    QString statement = QLatin1String("SELECT COUNT(*) FROM ") + OPL::GLOBALS->getDbTableName(row.getTable())
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

bool Database::clear()
{
    QSqlQuery q;

    for (const auto &table : USER_TABLES) {
        q.prepare(QLatin1String("DELETE FROM ") + OPL::GLOBALS->getDbTableName(table));
        if (!q.exec()) {
            DEB << "Error: " << q.lastError().text();
            lastError = q.lastError();
            return false;
        }
    }
    return true;
}

bool Database::update(const OPL::Row &updated_row)
{
    QString statement = QLatin1String("UPDATE ") + OPL::GLOBALS->getDbTableName(updated_row.getTable()) + QLatin1String(" SET ");
    const auto& data = updated_row.getData();
    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
        statement.append(i.key() + "=?,");
    }
    statement.chop(1);
    statement.append(QLatin1String(" WHERE ROWID=?"));
    QSqlQuery query;
    query.prepare(statement);
    DEB << "Statement: " << statement;
    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
//use QMetaType for binding null value in QT >= 6
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (i.value() == QVariant(QString())) {
            query.addBindValue(QVariant(QMetaType(QMetaType::Int)));
#else
        if (i.value() == QVariant(QString())) {
            query.addBindValue(QVariant(QVariant::String));
#endif
        } else {
            query.addBindValue(i.value());
        }

    }
    query.addBindValue(updated_row.getRowId());
    DEB << "Bound values: " << query.boundValues();

    if (query.exec())
    {
        LOG << QString("Entry successfully committed. %1").arg(updated_row.getPosition());
        emit dataBaseUpdated(updated_row.getTable());
        return true;
    } else {
        DEB << "Unable to commit.";
        DEB << "Query: " << statement;
        DEB << "Query Error: " << query.lastError().text();
        lastError = query.lastError();
        return false;
    }
}

bool Database::insert(const OPL::Row &new_row)
{
    QString statement = QLatin1String("INSERT INTO ") + OPL::GLOBALS->getDbTableName(new_row.getTable()) + QLatin1String(" (");
    const auto& data = new_row.getData();
    for(auto i = data.cbegin(); i != data.cend(); ++i) {
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

    for (auto i = data.constBegin(); i != data.constEnd(); ++i) {
//use QMetaType for binding null value in QT >= 6
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (i.value() == QVariant(QString())) {
            query.addBindValue(QVariant(QMetaType(QMetaType::Int)));
#else
        if (i.value() == QVariant(QString())) {
            query.addBindValue(QVariant(QVariant::String));
#endif
        } else {
            query.addBindValue(i.value());
        }
    }

    //check result.
    if (query.exec())
    {
        LOG << QString("Entry successfully committed. %1").arg(new_row.getPosition());
        emit dataBaseUpdated(new_row.getTable());
        return true;
    } else {
        DEB << "Unable to commit.";
        DEB << "Query: " << statement;
        DEB << "Bound Values: " << query.boundValues();
        DEB << "Query Error: " << query.lastError().text();
        lastError = query.lastError();
        return false;
    }
}

OPL::Row Database::getRow(const OPL::DbTable table, const int row_id)
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

RowData_T Database::getRowData(const OPL::DbTable table, const int row_id)
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

int Database::getLastEntry(OPL::DbTable table)
{
    QString statement = QLatin1String("SELECT MAX(ROWID) FROM ") + OPL::GLOBALS->getDbTableName(table);

    auto query = QSqlQuery(statement);
    if (query.first()) {
        return query.value(0).toInt();
    } else {
        LOG << "No entry found. (Database empty?)" << query.lastError().text();
        return 0;
    }
}

const RowData_T Database::getTotals(bool includePreviousExperience)
{
    QString statement = "SELECT"
        " SUM(tblk) AS tblk,"
        " SUM(tSPSE) AS tSPSE,"
        " SUM(tSPME) AS tSPME,"
        " SUM(tMP) AS tMP,"
        " SUM(tPIC) AS tPIC,"
        " SUM(tSIC) AS tSIC,"
        " SUM(tDUAL) AS tDUAL,"
        " SUM(tFI) AS tFI,"
        " SUM(tPICUS) AS tPICUS,"
        " SUM(tNIGHT) AS tNIGHT,"
        " SUM(tIFR) AS tIFR,"
        " SUM(tSIM) AS tSIM,"
        " SUM(toDay) AS toDay,"
        " SUM(toNight) AS toNight,"
        " SUM(ldgDay) AS ldgDay,"
        " SUM(ldgNight) AS ldgNight"
        " FROM flights";

    QSqlQuery query;
    query.prepare(statement);
    if (!query.exec()) {
        DEB << "SQL error: " << query.lastError().text();
        DEB << "Statement: " << query.lastQuery();
        lastError = query.lastError();
        return {}; // return invalid Row
    }

    RowData_T entry_data;
    if(query.next()) {
        auto r = query.record(); // retreive record
        if (r.count() == 0)  // row is empty
            return {};

        for (int i = 0; i < r.count(); i++){ // iterate through fields to get key:value map
            if(!r.value(i).isNull()) {
                entry_data.insert(r.fieldName(i), r.value(i));
            }
        }
    }

    if(!includePreviousExperience) {
        return entry_data;
    }

    // name the return types for easy mapping to QLineEdit names
    statement = "SELECT"
                " SUM(tblk) AS tblk,"
                " SUM(tSPSE) AS tSPSE,"
                " SUM(tSPME) AS tSPME,"
                " SUM(tMP) AS tMP,"
                " SUM(tPIC) AS tPIC,"
                " SUM(tSIC) AS tSIC,"
                " SUM(tDUAL) AS tDUAL,"
                " SUM(tFI) AS tFI,"
                " SUM(tPICUS) AS tPICUS,"
                " SUM(tNIGHT) AS tNIGHT,"
                " SUM(tIFR) AS tIFR,"
                " SUM(tSIM) AS tSIM,"
                " SUM(toDay) AS toDay,"
                " SUM(toNight) AS toNight,"
                " SUM(ldgDay) AS ldgDay,"
                " SUM(ldgNight) AS ldgNight"
                " FROM previousExperience";
    query.prepare(statement);

    if (!query.exec()) {
        DEB << "SQL error: " << query.lastError().text();
        DEB << "Statement: " << query.lastQuery();
        lastError = query.lastError();
        return {}; // return invalid Row
    }

    RowData_T prev_exp_data;
    if(query.next()) {
        auto r = query.record(); // retreive record
        if (r.count() == 0)  // row is empty
            return {};

        for (int i = 0; i < r.count(); i++){ // iterate through fields to get key:value map
            if(!r.value(i).isNull()) {
                prev_exp_data.insert(r.fieldName(i), r.value(i));
            }
        }
    }

    // add up the two query results
    for(auto it = prev_exp_data.begin(); it != prev_exp_data.end(); it++) {
        int prevXpValue = it.value().toInt();
        int entryValue = entry_data.value(it.key()).toInt();

        const QVariant sum = prevXpValue + entryValue;
        entry_data.insert(it.key(), sum);
    }

    return entry_data;
}

QList<int> Database::getForeignKeyConstraints(int foreign_row_id, OPL::DbTable table)
{
    QString statement = QLatin1String("SELECT ROWID FROM flights WHERE ");

    switch (table) {
    case OPL::DbTable::Pilots:
        statement.append(QLatin1String("pic=?"));
        break;
    case OPL::DbTable::Tails:
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

QVector<QVariant> Database::customQuery(QString statement, int return_values)
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

QVector<RowData_T> Database::getTable(OPL::DbTable table)
{
    const QString query_str = QStringLiteral("SELECT * FROM ") + GLOBALS->getDbTableName(table);

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

bool Database::createBackup(const QString& dest_file)
{
    LOG << "Backing up current database to: " << dest_file;
    Database::disconnect();
    QFile db_file(QDir::toNativeSeparators(databaseFile.absoluteFilePath()));

    if (!db_file.copy(QDir::toNativeSeparators(dest_file))) {
        LOG << "Unable to backup old database:" << db_file.errorString();
        return false;
    }

    LOG << "Backed up old database as:" << dest_file;
    Database::connect();
    emit connectionReset();
    return true;
}

bool Database::restoreBackup(const QString& backup_file)
{
    Database::disconnect();
    LOG << "Restoring backup from file:" << backup_file;

    QString databaseFilePath = QDir::toNativeSeparators(databaseFile.absoluteFilePath());
    DEB << "DB File Path: " << databaseFilePath;
    QString backupFilePath = QDir::toNativeSeparators(backup_file);

    QFile dbFile(databaseFilePath);
    if(dbFile.exists())
        if(!dbFile.remove()) {
            LOG << dbFile.errorString() << "Unable to remove current db file";
            return false;
        }

    QFile backupFile(backupFilePath);
    if(!backupFile.copy(databaseFilePath)) {
        LOG << backupFile.errorString() << "Could not copy" << backupFile.fileName() << " to " << databaseFilePath;
        return false;
    }

    LOG << "Backup successfully restored!";
    Database::connect();
    emit connectionReset();
    return true;
}

bool Database::createSchema()
{
    // Read Database layout from sql file
    QFile f(OPL::Assets::DATABASE_SCHEMA);
    if(!f.open(QIODevice::ReadOnly)) {
        LOG << "Unable to read database schema - " << f.errorString();
    }
    QByteArray filedata = f.readAll();
    // create individual queries for each table/view
    auto list = filedata.split(';');

    // make sure last empty line in sql file has not been parsed
    if(list.last() == QByteArray("\n") || list.last() == QByteArray("\r\n"))
        list.removeLast();

    // Create Tables
    QSqlQuery q;
    QVector<QSqlError> errors;
    for (const auto &query_string : std::as_const(list)) {
        q.prepare(query_string);
        if (!q.exec()) {
            errors.append(q.lastError());
            LOG << "Unable to execute query: ";
            LOG << q.lastQuery();
            LOG << q.lastError();
        }
    }
    DB->updateLayout();

    if (errors.isEmpty()) {
        LOG << "Database succesfully created.";
        return true;
    } else {
        LOG << "Database creation has failed. The following error(s) have ocurred: ";
        for (const auto &error : std::as_const(errors)) {
            LOG << error.type() << error.text();
        }
        return false;
    }
}

bool Database::importTemplateData(bool use_local_ressources)
{
    for (const auto& table : DB->getTemplateTables()) {
        const QString table_name = OPL::GLOBALS->getDbTableName(table);

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
            data_to_commit = JsonHelper::readFileToDoc(QLatin1String(":database/templates/")
                                      + table_name + QLatin1String(".json")).array();
            error_message.append(QLatin1String(" (ressource) "));
        } else {
            const QString file_path = OPL::Paths::filePath(OPL::Paths::Templates,
                                                           table_name + QLatin1String(".json"));
            data_to_commit = JsonHelper::readFileToDoc(file_path).array();
            //data_to_commit = AJson::readFileToDoc(AStandardPaths::directory(
            //                              AStandardPaths::Templates).absoluteFilePath(
            //                              table_name + QLatin1String(".json"))).array();
            error_message.append(QLatin1String(" (downloaded) "));
        }

        // commit Data from Array
        if (!DB->commit(data_to_commit, table)) {
            LOG << error_message;
            return false;
        }
    } // for table_name
    return true;
}

bool Database::resetUserData()
{
    QSqlQuery query;
    for (const auto& table : DB->getUserTables()) {
        query.prepare(QLatin1String("DELETE FROM ") + OPL::GLOBALS->getDbTableName(table));
        if (!query.exec()) {
            lastError = query.lastError();
            return false;
        }
    }
    return true;
}

} // namespace OPL
