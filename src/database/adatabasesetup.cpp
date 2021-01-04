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
#include "adatabasesetup.h"
#include "src/database/adatabase.h"
#include "src/testing/adebug.h"
#include "src/functions/areadcsv.h"
#include "src/classes/astandardpaths.h"
#include "src/classes/adownload.h"

// Statements for creation of database tables, Revision 15

const auto createTablePilots = QStringLiteral("CREATE TABLE pilots ( "
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

const auto createTableTails = QStringLiteral("CREATE TABLE tails ("
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

const auto createTableFlights = QStringLiteral("CREATE TABLE flights ("
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

const auto createTableAirports = QStringLiteral("CREATE TABLE airports ( "
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

const auto createTableAircraft = QStringLiteral("CREATE TABLE aircraft ("
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

const auto createTableChangelog = QStringLiteral("CREATE TABLE changelog ( "
            " revision   INTEGER NOT NULL, "
            " comment    TEXT, "
            " date       NUMERIC, "
            " PRIMARY KEY(revision) "
            ")");

// Statements for creation of views in the database
const auto createViewDefault = QStringLiteral("CREATE VIEW viewDefault AS "
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
        " make||' '||model||'-'||variant AS 'Type', "
        " registration AS 'Registration', "
        " FlightNumber AS 'Flight #', "
        " remarks AS 'Remarks'"
        " FROM flights "
        " INNER JOIN pilots on flights.pic = pilots.pilot_id "
        " INNER JOIN tails on flights.acft = tails.tail_id "
        " ORDER BY date DESC ");

const auto createViewEASA = QStringLiteral("CREATE VIEW viewEASA AS "
        " SELECT "
        " flight_id, doft as 'Date', "
        " dept AS 'Dept', "
        " printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time', "
        " dest AS 'Dest', printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time ', "
        " make||' '||model||'-'||variant AS 'Type', "
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

const auto createViewTails = QStringLiteral("CREATE VIEW viewTails AS "
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

const auto createViewPilots = QStringLiteral("CREATE VIEW viewPilots AS "
        " SELECT "
        " pilot_id AS 'ID', "
        " lastname AS 'Last Name', "
        " firstname AS 'First Name', "
        " company AS 'Company' "
        " FROM pilots");

const auto createViewQCompleter = QStringLiteral("CREATE VIEW viewQCompleter AS "
        " SELECT airport_id, icao, iata, tail_id, registration, pilot_id, "
        " lastname||', '||firstname AS 'pilot_name', alias "
        " FROM airports "
        " LEFT JOIN tails ON airports.airport_id = tails.tail_id "
        " LEFT JOIN pilots ON airports.airport_id = pilots.pilot_id");

const auto createViewTotals = QStringLiteral("CREATE VIEW viewTotals AS "
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
    "flights",
    "pilots",
    "tails"
};
const QStringList templateTables= {
    "aircraft",
    "airports",
    "changelog"
};


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

    DEB << "Populating tables...";
    if (!importDefaultData()) {
        DEB << "Populating tables failed.";
        return false;
    }

    DEB << "Database successfully created!";
    return true;
}

bool ADataBaseSetup::downloadTemplates()
{
    QDir template_dir(AStandardPaths::absPathOf(AStandardPaths::Templates));
    DEB << template_dir;
    for (const auto& table : templateTables) {
        QEventLoop loop;
        ADownload* dl = new ADownload;
        QObject::connect(dl, &ADownload::done, &loop, &QEventLoop::quit );
        dl->setTarget(QUrl(TEMPLATE_URL % table % QStringLiteral(".csv")));
        dl->setFileName(template_dir.filePath(table % QStringLiteral(".csv")));
        dl->download();
        loop.exec(); // event loop waits for download done signal before allowing loop to continue
        dl->deleteLater();
    }
    return true;
}
bool ADataBaseSetup::backupOldData()
{
    auto database_file = aDB()->databaseFile;
    if(!database_file.exists()) {
        DEB << "No Database to backup, returning.";
        return true;
    }

    auto date_string = QDateTime::currentDateTime().toString(Qt::ISODate);
    auto backup_dir = QDir(AStandardPaths::absPathOf(AStandardPaths::DatabaseBackup));
    auto backup_name = database_file.baseName() + "_bak_" + date_string + ".db";
    auto file = QFile(aDB()->databaseFile.absoluteFilePath());

    if (!file.rename(backup_dir.absolutePath() + '/' + backup_name)) {
        DEB << "Unable to backup old database.";
        return false;
    }
    DEB << "Backed up old database as: " << backup_name;
    return true;
}

bool ADataBaseSetup::importDefaultData()
{
    QSqlQuery query;
    // reset template tables
    for (const auto& table : templateTables) {
        //clear tables
        query.prepare("DELETE FROM " + table);
        if (!query.exec()) {
            DEB << "Error: " << query.lastError().text();
        }
        //fill with data from csv
        if (!commitData(aReadCsv(AStandardPaths::absPathOf(AStandardPaths::Templates)
                                 % QLatin1Char('/')
                                 % table % QStringLiteral(".csv")),
                        table)) {
            DEB << "Error importing data.";
            return false;
        }
    }
    return true;
};

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
        DEB_SRC << "The following errors have ocurred: ";
        for (const auto& error : errors) {
            DEB_RAW << error;
        }
        return false;
    }
    DEB << "All schemas added successfully";
    return true;
}
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
    DEB << "Table names: " << aDB()->getTableNames();
    DEB << "Importing Data to" << table_name;
    if (!aDB()->getTableNames().contains(table_name)){
        DEB << table_name << "is not a table in the database. Aborting.";
        DEB << "Please check input data.";
        return false;
    }
    // create insert statement
    QString statement = "INSERT INTO " + table_name + " (";
    QString placeholder = ") VALUES (";
    for (auto& csvColumn : from_csv) {
        if(aDB()->getTableColumns(table_name).contains(csvColumn.first())) {
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
                         query.addBindValue(QVariant(QVariant::String))
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
