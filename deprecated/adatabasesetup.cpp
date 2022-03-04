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
#include "adatabasesetup.h"
#include "src/database/adatabase.h"
#include "src/functions/alog.h"
#include "src/functions/areadcsv.h"
#include "src/classes/astandardpaths.h"
#include "src/classes/adownload.h"
#include "src/opl.h"
#include "src/functions/adatetime.h"
#include "src/functions/alog.h"

const auto createTablePilots = QLatin1String("CREATE TABLE pilots ( "
            " pilot_id       INTEGER NOT NULL, "
            " lastname       TEXT    NOT NULL, "
            " firstname      TEXT, "
            " alias          TEXT, "
            " company        TEXT, "
            " employeeid     TEXT, "
            " phone          TEXT, "
            " email          TEXT, "
            " PRIMARY KEY(pilot_id AUTOINCREMENT)"
            ")");

const auto createTableTails = QLatin1String("CREATE TABLE tails ("
            " tail_id        INTEGER NOT NULL,"
            " registration   TEXT NOT NULL,"
            " company        TEXT,"
            " make           TEXT,"
            " model          TEXT,"
            " variant        TEXT,"
            " multipilot     INTEGER,"
            " multiengine    INTEGER,"
            " engineType     INTEGER,"
            " weightClass    INTEGER,"
            " PRIMARY KEY(tail_id AUTOINCREMENT)"
            ")");

const auto createTableFlights = QLatin1String("CREATE TABLE flights ("
            " flight_id      INTEGER NOT NULL, "
            " doft           NUMERIC NOT NULL, "
            " dept           TEXT NOT NULL, "
            " dest           TEXT NOT NULL, "
            " tofb           INTEGER NOT NULL, "
            " tonb           INTEGER NOT NULL, "
            " pic            INTEGER NOT NULL, "
            " acft           INTEGER NOT NULL, "
            " tblk           INTEGER NOT NULL, "
            " tSPSE          INTEGER, "
            " tSPME          INTEGER, "
            " tMP            INTEGER, "
            " tNIGHT         INTEGER, "
            " tIFR           INTEGER, "
            " tPIC           INTEGER, "
            " tPICUS         INTEGER, "
            " tSIC           INTEGER, "
            " tDUAL          INTEGER, "
            " tFI            INTEGER, "
            " tSIM           INTEGER, "
            " pilotFlying    INTEGER, "
            " toDay          INTEGER, "
            " toNight        INTEGER, "
            " ldgDay         INTEGER, "
            " ldgNight       INTEGER, "
            " autoland       INTEGER, "
            " secondPilot    INTEGER, "
            " thirdPilot     INTEGER, "
            " approachType   TEXT, "
            " flightNumber   TEXT, "
            " remarks        TEXT, "
            " FOREIGN KEY(pic)  REFERENCES pilots(pilot_id) ON DELETE RESTRICT, "
            " FOREIGN KEY(acft) REFERENCES tails(tail_id)   ON DELETE RESTRICT, "
            " PRIMARY KEY(flight_id    AUTOINCREMENT) "
        ")");

const auto createTableAirports = QLatin1String("CREATE TABLE airports ( "
            " airport_id     INTEGER NOT NULL, "
            " icao           TEXT NOT NULL, "
            " iata           TEXT, "
            " name           TEXT, "
            " lat            REAL, "
            " long           REAL, "
            " country        TEXT, "
            " alt            INTEGER, "
            " utcoffset      INTEGER, "
            " tzolson        TEXT, "
            " PRIMARY KEY(airport_id AUTOINCREMENT) "
            ")");

const auto createTableAircraft = QLatin1String("CREATE TABLE aircraft ("
            " aircraft_id   INTEGER NOT NULL,"
            " make          TEXT,"
            " model         TEXT,"
            " variant       TEXT,"
            " name          TEXT,"
            " iata          TEXT,"
            " icao          TEXT,"
            " multipilot    INTEGER,"
            " multiengine   INTEGER,"
            " engineType    INTEGER,"
            " weightClass   INTEGER,"
            " PRIMARY KEY(aircraft_id AUTOINCREMENT)"
            ")");

const auto createTableChangelog = QLatin1String("CREATE TABLE changelog ( "
            " revision   INTEGER NOT NULL, "
            " comment    TEXT, "
            " date       NUMERIC, "
            " PRIMARY KEY(revision) "
            ")");

const auto createTableCurrencies = QLatin1String("CREATE TABLE currencies ( "
            " currency_id	INTEGER PRIMARY KEY AUTOINCREMENT, "
            " description	TEXT, "
            " expiryDate     NUMERIC "
            ")"
            );

// Statements for creation of views in the database
const auto createViewDefault = QLatin1String("CREATE VIEW viewDefault AS "
        " SELECT flight_id, doft as 'Date', "
        " dept AS 'Dept', "
        " printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time', "
        " dest AS 'Dest', printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time ', "
        " printf('%02d',(tblk/60))||':'||printf('%02d',(tblk%60)) AS 'Total', "
        " CASE "
        " WHEN pilot_id = 1 THEN alias "
        " ELSE lastname||', '||substr(firstname, 1, 1)||'.' "
        " END "
        " AS 'Name PIC', "
        " CASE "
        " WHEN variant IS NOT NULL THEN make||' '||model||'-'||variant "
        " ELSE make||' '||model "
        " END "
        " AS 'Type', "
        " registration AS 'Registration', "
        " FlightNumber AS 'Flight #', "
        " remarks AS 'Remarks'"
        " FROM flights "
        " INNER JOIN pilots on flights.pic = pilots.pilot_id "
        " INNER JOIN tails on flights.acft = tails.tail_id "
        " ORDER BY date DESC ");

const auto createViewEASA = QLatin1String("CREATE VIEW viewEASA AS "
        " SELECT "
        " flight_id, doft as 'Date', "
        " dept AS 'Dept', "
        " printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time', "
        " dest AS 'Dest', printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time ', "
        " CASE "
        " WHEN variant IS NOT NULL THEN make||' '||model||'-'||variant "
        " ELSE make||' '||model "
        " END "
        " AS 'Type', "
        " registration AS 'Registration', "
        " (SELECT printf('%02d',(tSPSE/60))||':'||printf('%02d',(tSPSE%60)) WHERE tSPSE IS NOT NULL) AS 'SP SE', "
        " (SELECT printf('%02d',(tSPME/60))||':'||printf('%02d',(tSPME%60)) WHERE tSPME IS NOT NULL) AS 'SP ME', "
        " (SELECT printf('%02d',(tMP/60))||':'||printf('%02d',(tMP%60)) WHERE tMP IS NOT NULL) AS 'MP', "
        " printf('%02d',(tblk/60))||':'||printf('%02d',(tblk%60)) AS 'Total', "
        " CASE "
        " WHEN pilot_id = 1 THEN alias "
        " ELSE lastname||', '||substr(firstname, 1, 1)||'.' "
        " END "
        " AS 'Name PIC', "
        " ldgDay AS 'L/D', "
        " ldgNight AS 'L/N', "
        " (SELECT printf('%02d',(tNight/60))||':'||printf('%02d',(tNight%60)) WHERE tNight IS NOT NULL)  AS 'Night', "
        " (SELECT printf('%02d',(tIFR/60))||':'||printf('%02d',(tIFR%60)) WHERE tIFR IS NOT NULL)  AS 'IFR', "
        " (SELECT printf('%02d',(tPIC/60))||':'||printf('%02d',(tPIC%60)) WHERE tPIC IS NOT NULL)  AS 'PIC', "
        " (SELECT printf('%02d',(tSIC/60))||':'||printf('%02d',(tSIC%60)) WHERE tSIC IS NOT NULL)  AS 'SIC', "
        " (SELECT printf('%02d',(tDual/60))||':'||printf('%02d',(tDual%60)) WHERE tDual IS NOT NULL)  AS 'Dual', "
        " (SELECT printf('%02d',(tFI/60))||':'||printf('%02d',(tFI%60)) WHERE tFI IS NOT NULL)  AS 'FI', "
        " remarks AS 'Remarks' "
        " FROM flights "
        " INNER JOIN pilots on flights.pic = pilots.pilot_id "
        " INNER JOIN tails on flights.acft = tails.tail_id "
        " ORDER BY date DESC");

const auto createViewTails = QLatin1String("CREATE VIEW viewTails AS "
        " SELECT "
        " tail_id AS 'ID', "
        " registration AS 'Registration', "
        " make||' '||model AS 'Type', "
        " company AS 'Company' "
        " FROM tails WHERE model IS NOT NULL AND variant IS NULL "
        " UNION "
        " SELECT "
        " tail_id AS 'ID', "
        " registration AS 'Registration', "
        " make||' '||model||'-'||variant AS 'Type', "
        " company AS 'Company' "
        " FROM tails WHERE variant IS NOT NULL");

const auto createViewPilots = QLatin1String("CREATE VIEW viewPilots AS "
        " SELECT "
        " pilot_id AS 'ID', "
        " lastname AS 'Last Name', "
        " firstname AS 'First Name', "
        " company AS 'Company' "
        " FROM pilots");

const auto createViewQCompleter = QLatin1String("CREATE VIEW viewQCompleter AS "
        " SELECT airport_id, icao, iata, tail_id, registration, pilot_id, "
        " lastname||', '||firstname AS 'pilot_name', alias "
        " FROM airports "
        " LEFT JOIN tails ON airports.airport_id = tails.tail_id "
        " LEFT JOIN pilots ON airports.airport_id = pilots.pilot_id");

const auto createViewTotals = QLatin1String("CREATE VIEW viewTotals AS "
        " SELECT "
        " printf(\"%02d\",CAST(SUM(tblk) AS INT)/60)||\":\"||printf(\"%02d\",CAST(SUM(tblk) AS INT)%60) AS \"TOTAL\", "
        " printf(\"%02d\",CAST(SUM(tSPSE) AS INT)/60)||\":\"||printf(\"%02d\",CAST(SUM(tSPSE) AS INT)%60) AS \"SP SE\", "
        " printf(\"%02d\",CAST(SUM(tSPME) AS INT)/60)||\":\"||printf(\"%02d\",CAST(SUM(tSPME) AS INT)%60) AS \"SP ME\", "
        " printf(\"%02d\",CAST(SUM(tNIGHT) AS INT)/60)||\":\"||printf(\"%02d\",CAST(SUM(tNIGHT) AS INT)%60) AS \"NIGHT\", "
        " printf(\"%02d\",CAST(SUM(tIFR) AS INT)/60)||\":\"||printf(\"%02d\",CAST(SUM(tIFR) AS INT)%60) AS \"IFR\", "
        " printf(\"%02d\",CAST(SUM(tPIC) AS INT)/60)||\":\"||printf(\"%02d\",CAST(SUM(tPIC) AS INT)%60) AS \"PIC\", "
        " printf(\"%02d\",CAST(SUM(tPICUS) AS INT)/60)||\":\"||printf(\"%02d\",CAST(SUM(tPICUS) AS INT)%60) AS \"PICUS\", "
        " printf(\"%02d\",CAST(SUM(tSIC) AS INT)/60)||\":\"||printf(\"%02d\",CAST(SUM(tSIC) AS INT)%60) AS \"SIC\", "
        " printf(\"%02d\",CAST(SUM(tDual) AS INT)/60)||\":\"||printf(\"%02d\",CAST(SUM(tDual) AS INT)%60) AS \"DUAL\", "
        " printf(\"%02d\",CAST(SUM(tFI) AS INT)/60)||\":\"||printf(\"%02d\",CAST(SUM(tFI) AS INT)%60) AS \"INSTRUCTOR\", "
        " printf(\"%02d\",CAST(SUM(tSIM) AS INT)/60)||\":\"||printf(\"%02d\",CAST(SUM(tSIM) AS INT)%60) AS \"SIMULATOR\", "
        " printf(\"%02d\",CAST(SUM(tMP) AS INT)/60)||\":\"||printf(\"%02d\",CAST(SUM(tMP) AS INT)%60) AS \"MultPilot\", "
        " CAST(SUM(toDay) AS INT) AS \"TO Day\", CAST(SUM(toNight) AS INT) AS \"TO Night\", "
        " CAST(SUM(ldgDay) AS INT) AS \"LDG Day\", CAST(SUM(ldgNight) AS INT) AS \"LDG Night\" "
        " FROM flights");

const QStringList tables = {
    createTablePilots,
    createTableTails,
    createTableFlights,
    createTableAircraft,
    createTableAirports,
    createTableCurrencies,
    createTableChangelog
};
const QStringList views = {
    createViewDefault,
    createViewEASA,
    createViewTails,
    createViewPilots,
    createViewTotals,
    createViewQCompleter,
};
const QStringList userTables = {
    QStringLiteral("flights"),
    QStringLiteral("pilots"),
    QStringLiteral("tails")
};
const QStringList templateTables= {
    QStringLiteral("aircraft"),
    QStringLiteral("airports"),
    QStringLiteral("currencies"),
    QStringLiteral("changelog")
};

QT_DEPRECATED
bool ADataBaseSetup::createDatabase()
{

    DEB << "Creating tables...";
    if (!createSchemata(tables)) {
        DEB << "Creating tables has failed.";
        return false;
    }

    DEB << "Creating views...";
    if (!createSchemata(views)) {
        DEB << "Creating views failed.";
        return false;
    }

    aDB->updateLayout();

    LOG << "Database successfully created!\n";
    return true;
}

QT_DEPRECATED
bool ADataBaseSetup::downloadTemplates()
{
    QDir template_dir(AStandardPaths::directory(AStandardPaths::Templates));
    DEB << template_dir;
    for (const auto& table : templateTables) {
        QEventLoop loop;
        ADownload* dl = new ADownload;
        QObject::connect(dl, &ADownload::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(TEMPLATE_URL + table + QLatin1String(".csv")));
        dl->setFileName(template_dir.absoluteFilePath(table + QLatin1String(".csv")));
        dl->download();
        dl->deleteLater();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue

        QFileInfo downloaded_file(template_dir.filePath(table + QLatin1String(".csv")));
        if (downloaded_file.size() == 0)
            return false; // ssl/network error
    }
    return true;
}

QT_DEPRECATED
bool ADataBaseSetup::backupOldData()
{
    LOG << "Backing up old database...";
    QFileInfo database_file(AStandardPaths::directory(AStandardPaths::Database).
                                       absoluteFilePath(QStringLiteral("logbook.db")));
    DEB << "File Info:" << database_file;

    if(!database_file.exists()) {
        DEB << "No Database to backup, returning.";
        return true;
    }
    auto date_string = ADateTime::toString(QDateTime::currentDateTime(),
                                           Opl::Datetime::Backup);
    auto backup_dir = AStandardPaths::directory(AStandardPaths::Backup);
    QString backup_name = database_file.baseName() + QLatin1String("_bak_")
            + date_string + QLatin1String(".db");
    QFile file(database_file.absoluteFilePath());
    DEB << "File:" << file.fileName();
    if (!file.rename(backup_dir.absoluteFilePath(backup_name))) {
        LOG << "Unable to backup old database.\n";
        return false;
    }
    LOG << "Backed up old database as: " << backup_name << "\n";
    return true;
}

QT_DEPRECATED
bool ADataBaseSetup::importDefaultData(bool use_ressource_data)
{
    QSqlQuery query;
    // reset template tables
    for (const auto& table_name : templateTables) {
        //clear tables
        query.prepare("DELETE FROM " + table_name);
        if (!query.exec()) {
            DEB << "Error: " << query.lastError().text();
            return false;
        }
        // Prepare data
        QVector<QStringList> data_to_commit;
        QString error_message("Error importing data ");

        if (use_ressource_data) {
            data_to_commit = aReadCsv(QStringLiteral(":templates/database/templates/")
                                      + table_name + QLatin1String(".csv"));
            error_message.append(" (ressource) ");
        } else {
            data_to_commit = aReadCsv(AStandardPaths::directory(
                                          AStandardPaths::Templates).absoluteFilePath(
                                          table_name + QLatin1String(".csv")));
            error_message.append(" (downloaded) ");
        }

        //fill with data from csv
        if (!commitData(data_to_commit, table_name)) {
            LOG << error_message;
            return false;
        }
    }

    return true;
};

QT_DEPRECATED
/*!
 * \brief DbSetup::resetToDefault Empties all user-generated content in the database.
 * \return true on success
 */
bool ADataBaseSetup::resetToDefault()
{
    QSqlQuery query;

    // clear user tables
    for (const auto& table : userTables) {
        query.prepare("DELETE FROM " + table);
        if (!query.exec()) {
            DEB << "Error: " << query.lastError().text();
        }
    }
    return true;
}

QT_DEPRECATED
/*!
 * \brief dbSetup::debug prints Database Layout
 */
void ADataBaseSetup::debug()
{
    DEB << "Database tables and views: ";
    QSqlQuery query;
    const QVector<QString> types = { "table", "view" };
    for (const auto& var : types){
        query.prepare("SELECT name FROM sqlite_master WHERE type=" + var);
        query.exec();
        while (query.next()) {
            QString table = query.value(0).toString();
            QSqlQuery entries("SELECT COUNT(*) FROM " + table);
            entries.next();
            DEB << "Element " << query.value(0).toString() << "with"
                << entries.value(0).toString() << "rows";
        }
    }
}

QT_DEPRECATED
/*!
 * \brief dbSetup::createTables Create the required tables for the database
 * \return true on success
 */
bool ADataBaseSetup::createSchemata(const QStringList &statements)
{
    QSqlQuery query;
    QStringList errors;

    for (const auto& statement : statements) {
        query.prepare(statement);
        query.exec();
        if(!query.isActive()) {
            errors << statement.section(QLatin1Char(' '),2,2) + " ERROR - " + query.lastError().text();
            DEB << "Query: " << query.lastQuery();
            continue;
        }
        DEB << "Schema added: " << statement.section(QLatin1Char(' '), 2, 2);
    }

    if (!errors.isEmpty()) {
        DEB << "The following errors have ocurred: ";
        for (const auto& error : qAsConst(errors)) {
            DEB << error;
        }
        return false;
    }
    LOG << "All database tables created successfully\n";
    return true;
}
QT_DEPRECATED
/*!
 * \brief DbSetup::commitData inserts the data parsed from a csv file into the
 * database. The first line of the csv file has to contain the column names
 * of the corresponding table in the database.
 * \param fromCSV input as parsed from CSV::read()
 * \param tableName as in the database
 * \return
 */
bool ADataBaseSetup::commitData(QVector<QStringList> from_csv, const QString &table_name)
{
    aDB->updateLayout();
    if (!aDB->getTableNames().contains(table_name)){
        DEB << table_name << "is not a table in the database. Aborting.";
        DEB << "Please check input data.";
        return false;
    }
    // create insert statement
    QString statement = "INSERT INTO " + table_name + " (";
    QString placeholder = ") VALUES (";
    for (auto& csvColumn : from_csv) {
        if(aDB->getTableColumns(table_name).contains(csvColumn.first())) {
            statement += csvColumn.first() + ',';
            csvColumn.removeFirst();
            placeholder.append("?,");
        } else {
            DEB << csvColumn.first() << "is not a column of " << table_name << "Aborting.";
            DEB << "Please check input data.";
            return false;
        }
    }
    statement.chop(1);
    placeholder.chop(1);
    placeholder.append(')');
    statement.append(placeholder);

    /*
     * Using exclusive transaction and the loop below is MUCH faster than
     * passing the QStringLists to QSqlQuery::addBindValue and using QSqlQuery::execBatch()
     */
    QSqlQuery query;
    query.exec("BEGIN EXCLUSIVE TRANSACTION;");
    for (int i = 0; i < from_csv.first().length(); i++){
        query.prepare(statement);
        for(int j = 0; j < from_csv.length(); j++) {
             from_csv[j][i] == QString("") ? // make sure NULL is committed for empty values
                         query.addBindValue(QVariant(QString()))
                       : query.addBindValue(from_csv[j][i]);
             //query.addBindValue(fromCSV[j][i]);
         }
        query.exec();
    }

    query.exec("COMMIT;"); //commit transaction
    if (query.lastError().text().length() > 3) {
        DEB << "Error:" << query.lastError().text();
        return false;
    } else {
        qDebug() << table_name << "Database successfully updated!";
        return true;
    }
}

QT_DEPRECATED
bool ADataBaseSetup::commitDataJson(const QJsonArray &json_arr, const QString &table_name)
{
    aDB->updateLayout();
    QSqlQuery q;

    // create insert statement
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

    q.prepare(QStringLiteral("BEGIN EXCLUSIVE TRANSACTION"));
    q.exec();
    //DEB << statement;
    for (const auto &entry : json_arr) {
        q.prepare(statement);

        auto object = entry.toObject();
        const auto keys = object.keys();
        for (const auto &key : keys){
            object.value(key).isNull() ? q.bindValue(key, QVariant(QVariant::String)) :
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
