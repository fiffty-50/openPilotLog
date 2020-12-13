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
#include "src/functions/adebug.h"


// Statements for creation of database tables, Revision 12

const QString createTablePilots = "CREATE TABLE \"pilots\" ( "
            "\"pilot_id\"       INTEGER NOT NULL, "
            "\"piclastname\"    TEXT    NOT NULL, "
            "\"picfirstname\"   TEXT, "
            "\"alias\"          TEXT, "
            "\"company\"        TEXT, "
            "\"employeeid\"     TEXT, "
            "\"phone\"          TEXT, "
            "\"email\"          TEXT, "
            "PRIMARY KEY(\"pilot_id\" AUTOINCREMENT)"
            ")";

const QString createTableTails = "CREATE TABLE \"tails\" ( "
            "\"tail_id\"        INTEGER NOT NULL, "
            "\"registration\"   TEXT NOT NULL, "
            "\"company\"        TEXT, "
            "\"make\"           TEXT, "
            "\"model\"          TEXT, "
            "\"variant\"        TEXT, "
            "\"singlepilot\"    INTEGER, "
            "\"multipilot\"     INTEGER, "
            "\"singleengine\"   INTEGER, "
            "\"multiengine\"    INTEGER, "
            "\"unpowered\"      INTEGER, "
            "\"piston\"         INTEGER, "
            "\"turboprop\"      INTEGER, "
            "\"jet\"            INTEGER, "
            "\"light\"          INTEGER, "
            "\"medium\"         INTEGER, "
            "\"heavy\"          INTEGER, "
            "\"super\"          INTEGER, "
            "PRIMARY KEY(\"tail_id\" AUTOINCREMENT)"
            ")";

const QString createTableFlights = "CREATE TABLE \"flights\" ("
            "\"flight_id\"      INTEGER NOT NULL, "
            "\"doft\"           NUMERIC NOT NULL, "
            "\"dept\"           TEXT NOT NULL, "
            "\"dest\"           TEXT NOT NULL, "
            "\"tofb\"           INTEGER NOT NULL, "
            "\"tonb\"           INTEGER NOT NULL, "
            "\"pic\"            INTEGER NOT NULL, "
            "\"acft\"           INTEGER NOT NULL, "
            "\"tblk\"           INTEGER NOT NULL, "
            "\"tSPSE\"          INTEGER, "
            "\"tSPME\"          INTEGER, "
            "\"tMP\"            INTEGER, "
            "\"tNIGHT\"         INTEGER, "
            "\"tIFR\"           INTEGER, "
            "\"tPIC\"           INTEGER, "
            "\"tPICUS\"         INTEGER, "
            "\"tSIC\"           INTEGER, "
            "\"tDUAL\"          INTEGER, "
            "\"tFI\"            INTEGER, "
            "\"tSIM\"           INTEGER, "
            "\"pilotFlying\"    INTEGER, "
            "\"toDay\"          INTEGER, "
            "\"toNight\"        INTEGER, "
            "\"ldgDay\"         INTEGER, "
            "\"ldgNight\"       INTEGER, "
            "\"autoland\"       INTEGER, "
            "\"secondPilot\"    INTEGER, "
            "\"thirdPilot\"     INTEGER, "
            "\"ApproachType\"   TEXT, "
            "\"FlightNumber\"   TEXT, "
            "\"Remarks\"        TEXT, "
            "FOREIGN KEY(\"pic\")  REFERENCES \"pilots\"(\"pilot_id\") ON DELETE RESTRICT, "
            "FOREIGN KEY(\"acft\") REFERENCES \"tails\"(\"tail_id\")   ON DELETE RESTRICT, "
            "PRIMARY KEY(\"flight_id\"    AUTOINCREMENT) "
        ")";

const QString createTableAirports = "CREATE TABLE \"airports\" ( "
            "\"airport_id\"     INTEGER NOT NULL, "
            "\"icao\"           TEXT NOT NULL, "
            "\"iata\"           TEXT, "
            "\"name\"           TEXT, "
            "\"lat\"            REAL, "
            "\"long\"           REAL, "
            "\"country\"        TEXT, "
            "\"alt\"            INTEGER, "
            "\"utcoffset\"      INTEGER, "
            "\"tzolson\"        TEXT, "
            "PRIMARY KEY(\"airport_id\" AUTOINCREMENT) "
            ")";

const QString createTableAircraft = "CREATE TABLE \"aircraft\" ( "
            "\"aircraft_id\"    INTEGER NOT NULL, "
            "\"make\"           TEXT, "
            "\"model\"          TEXT, "
            "\"variant\"        TEXT, "
            "\"name\"           TEXT, "
            "\"iata\"           TEXT, "
            "\"icao\"           TEXT, "
            "\"singlepilot\"    INTEGER, "
            "\"multipilot\"     INTEGER, "
            "\"singleengine\"   INTEGER, "
            "\"multiengine\"    INTEGER, "
            "\"unpowered\"      INTEGER, "
            "\"piston\"         INTEGER, "
            "\"turboprop\"      INTEGER, "
            "\"jet\"            INTEGER, "
            "\"light\"          INTEGER, "
            "\"medium\"         INTEGER, "
            "\"heavy\"          INTEGER, "
            "\"super\"          INTEGER, "
            "PRIMARY KEY(\"aircraft_id\" AUTOINCREMENT)"
            ")";

const QString createTableChangelog = "CREATE TABLE \"changelog\" ( "
            "\"revision\"   INTEGER NOT NULL, "
            "\"comment\"    TEXT, "
            "\"date\"       NUMERIC, "
            "PRIMARY KEY(\"revision\") "
            ")";

// Statements for creation of views in the database
const QString createViewDefault = "CREATE VIEW viewDefault AS "
        "SELECT flight_id, doft as 'Date', "
        "dept AS 'Dept', "
        "printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time', "
        "dest AS 'Dest', printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time ', "
        "printf('%02d',(tblk/60))||':'||printf('%02d',(tblk%60)) AS 'Total', "
        "CASE "
        "WHEN pilot_id = 1 THEN alias "
        "ELSE piclastname||', '||substr(picfirstname, 1, 1)||'.' "
        "END "
        "AS 'Name PIC', "
        "make||' '||model||'-'||variant AS 'Type', "
        "registration AS 'Registration', "
        "FlightNumber AS 'Flight #', "
        "Remarks "
        "FROM flights "
        "INNER JOIN pilots on flights.pic = pilots.pilot_id "
        "INNER JOIN tails on flights.acft = tails.tail_id "
        "ORDER BY date DESC ";



const QString createViewEASA = "CREATE VIEW viewEASA AS "
        "SELECT "
        "flight_id, doft as 'Date', "
        "dept AS 'Dept', "
        "printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time', "
        "dest AS 'Dest', printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time ', "
        "make||' '||model||'-'||variant AS 'Type', "
        "registration AS 'Registration', "
        "(SELECT printf('%02d',(tSPSE/60))||':'||printf('%02d',(tSPSE%60)) WHERE tSPSE IS NOT \"\") AS 'SP SE', "
        "(SELECT printf('%02d',(tSPME/60))||':'||printf('%02d',(tSPME%60)) WHERE tSPME IS NOT \"\") AS 'SP ME', "
        "(SELECT printf('%02d',(tMP/60))||':'||printf('%02d',(tMP%60)) WHERE tMP IS NOT \"\") AS 'MP', "
        "printf('%02d',(tblk/60))||':'||printf('%02d',(tblk%60)) AS 'Total', "
        "CASE "
        "WHEN pilot_id = 1 THEN alias "
        "ELSE piclastname||', '||substr(picfirstname, 1, 1)||'.' "
        "END "
        "AS 'Name PIC', "
        "ldgDay AS 'L/D', "
        "ldgNight AS 'L/N', "
        "(SELECT printf('%02d',(tNight/60))||':'||printf('%02d',(tNight%60)) WHERE tNight IS NOT \"\")  AS 'Night', "
        "(SELECT printf('%02d',(tIFR/60))||':'||printf('%02d',(tIFR%60)) WHERE tIFR IS NOT \"\")  AS 'IFR', "
        "(SELECT printf('%02d',(tPIC/60))||':'||printf('%02d',(tPIC%60)) WHERE tPIC IS NOT \"\")  AS 'PIC', "
        "(SELECT printf('%02d',(tSIC/60))||':'||printf('%02d',(tSIC%60)) WHERE tSIC IS NOT \"\")  AS 'SIC', "
        "(SELECT printf('%02d',(tDual/60))||':'||printf('%02d',(tDual%60)) WHERE tDual IS NOT \"\")  AS 'Dual', "
        "(SELECT printf('%02d',(tFI/60))||':'||printf('%02d',(tFI%60)) WHERE tFI IS NOT \"\")  AS 'FI', "
        "Remarks "
        "FROM flights "
        "INNER JOIN pilots on flights.pic = pilots.pilot_id "
        "INNER JOIN tails on flights.acft = tails.tail_id "
        "ORDER BY date DESC";

const QString createViewTails = "CREATE VIEW viewTails AS "
        "SELECT "
        "tail_id AS 'ID', registration AS 'Registration', "
        "make||' '||model||'-'||variant AS 'Type', "
        "company AS 'Company' "
        "FROM tails";

const QString createViewPilots = "CREATE VIEW viewPilots AS "
        "SELECT "
        "pilot_id AS 'ID', "
        "piclastname AS 'Last Name', "
        "picfirstname AS 'First Name', company AS 'Company' "
        "FROM pilots";

const QString createViewQCompleter = "CREATE VIEW viewQCompleter AS "
        "SELECT airport_id, icao, iata, tail_id, registration, pilot_id, "
        "piclastname||', '||picfirstname AS 'pilot_name', alias "
        "FROM airports "
        "LEFT JOIN tails ON airports.airport_id = tails.tail_id "
        "LEFT JOIN pilots ON airports.airport_id = pilots.pilot_id";

const QString createViewTotals = "CREATE VIEW viewTotals AS "
        "SELECT "
        "printf('%02d',CAST(SUM(tblk) AS INT)/60)||':'||printf('%02d',CAST(SUM(tblk) AS INT)%60) AS 'TOTAL', "
        "printf('%02d',CAST(SUM(tSPSE) AS INT)/60)||':'||printf('%02d',CAST(SUM(tSPSE) AS INT)%60) AS 'SP SE', "
        "printf('%02d',CAST(SUM(tSPME) AS INT)/60)||':'||printf('%02d',CAST(SUM(tSPME) AS INT)%60) AS 'SP ME', "
        "printf('%02d',CAST(SUM(tNIGHT) AS INT)/60)||':'||printf('%02d',CAST(SUM(tNIGHT) AS INT)%60) AS 'NIGHT', "
        "printf('%02d',CAST(SUM(tIFR) AS INT)/60)||':'||printf('%02d',CAST(SUM(tIFR) AS INT)%60) AS 'IFR', "
        "printf('%02d',CAST(SUM(tPIC) AS INT)/60)||':'||printf('%02d',CAST(SUM(tPIC) AS INT)%60) AS 'PIC', "
        "printf('%02d',CAST(SUM(tPICUS) AS INT)/60)||':'||printf('%02d',CAST(SUM(tPICUS) AS INT)%60) AS 'PICUS', "
        "printf('%02d',CAST(SUM(tSIC) AS INT)/60)||':'||printf('%02d',CAST(SUM(tSIC) AS INT)%60) AS 'SIC', "
        "printf('%02d',CAST(SUM(tDual) AS INT)/60)||':'||printf('%02d',CAST(SUM(tDual) AS INT)%60) AS 'DUAL', "
        "printf('%02d',CAST(SUM(tFI) AS INT)/60)||':'||printf('%02d',CAST(SUM(tFI) AS INT)%60) AS 'INSTRUCTOR', "
        "printf('%02d',CAST(SUM(tSIM) AS INT)/60)||':'||printf('%02d',CAST(SUM(tSIM) AS INT)%60) AS 'SIMULATOR', "
        "printf('%02d',CAST(SUM(tMP) AS INT)/60)||':'||printf('%02d',CAST(SUM(tMP) AS INT)%60) AS 'MultPilot', "
        "CAST(SUM(toDay) AS INT) AS 'TO Day', CAST(SUM(toNight) AS INT) AS 'TO Night', "
        "CAST(SUM(ldgDay) AS INT) AS 'LDG Day', CAST(SUM(ldgNight) AS INT) AS 'LDG Night' "
        "FROM flights";
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
    createViewQCompleter
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
    DEB("Creating tables...");
    if (!createSchemata(tables)) {
        DEB("Creating tables has failed.");
        return false;
    }

    DEB("Creating views...");
    if (!createSchemata(views)) {
        DEB("Creating views failed.");
        return false;
    }

    DEB("Populating tables...");
    if (!importDefaultData()) {
        DEB("Populating tables failed.");
        return false;
    }

    DEB("Database successfully created!");
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
            DEB("Error: " << query.lastError().text());
        }
        //fill with data from csv
        if (!commitData(aReadCsv("data/templates/" + table + ".csv"), table)) {
            DEB("Error importing data.");
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
            DEB("Error: " << query.lastError().text());
        }
    }
    return true;
}

/*!
 * \brief dbSetup::debug prints Database Layout
 */
void ADataBaseSetup::debug()
{
    DEB("Database tables and views: ");
    QSqlQuery query;
    const QVector<QString> types = { "\"table\"", "\"view\"" };
    for (const auto& var : types){
        query.prepare("SELECT name FROM sqlite_master WHERE type=" + var);
        query.exec();
        while (query.next()) {
            QString table = query.value(0).toString();
            QSqlQuery entries("SELECT COUNT(*) FROM " + table);
            entries.next();
            DEB("Element " << query.value(0).toString()) << "with"
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
        } else {
            DEB("Schema added: " << statement.section(QLatin1Char(' '),2,2));
        }
    }

    if (!errors.isEmpty()) {
        DEB("The following errors have ocurred: ");
        for (const auto& error : errors) {
            DEB(error);
        }
        return false;
    } else {
        DEB("All schemas added successfully");
        return true;
    }
}
/*!
 * \brief DbSetup::commitData inserts the data parsed from a csv file into the
 * database. The first line of the csv file has to contain the column names
 * of the corresponding table in the database.
 * \param fromCSV input as parsed from CSV::read()
 * \param tableName as in the database
 * \return
 */
bool ADataBaseSetup::commitData(QVector<QStringList> fromCSV, const QString &tableName)
{
    DEB("Importing Data to" << tableName);
    auto dbLayout = DbInfo();
    if (!dbLayout.tables.contains(tableName)){
        DEB(tableName << "is not a table in the database. Aborting.");
        DEB("Please check input data.");
        return false;
    }
    // create insert statement
    QString statement = "INSERT INTO " + tableName + " (";
    QString placeholder = ") VALUES (";
    for (auto& csvColumn : fromCSV) {
        if(dbLayout.format.value(tableName).contains(csvColumn.first())){
            statement += csvColumn.first() + ',';
            csvColumn.removeFirst();
            placeholder.append("?,");
        } else {
            DEB(csvColumn.first() << "is not a column of " << tableName << "Aborting.");
            DEB("Please check input data.");
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
    for (int i = 0; i < fromCSV.first().length(); i++){
        query.prepare(statement);
        for(int j = 0; j < fromCSV.length(); j++) {
            query.addBindValue(fromCSV[j][i]);
        }
        query.exec();
    }

    query.exec("COMMIT;"); //commit transaction
    if (query.lastError().text().length() > 3) {
        DEB("Error:" << query.lastError().text());
        return false;
    } else {
        qDebug() << tableName << "Database successfully updated!";
        return true;
    }
}
