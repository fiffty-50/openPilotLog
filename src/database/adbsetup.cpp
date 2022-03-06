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
#include "adbsetup.h"
#include "src/opl.h"
#include "src/database/adatabase.h"
#include "src/functions/alog.h"
#include "src/classes/ajson.h"

namespace aDbSetup {

// const auto TEMPLATE_URL = QStringLiteral("https://raw.githubusercontent.com/fiffty-50/openpilotlog/tree/main/assets/database/templates/");
const static auto TEMPLATE_URL = QStringLiteral("https://raw.githubusercontent.com/fiffty-50/openpilotlog/develop/assets/database/templates/");


const static auto CREATE_TABLE_PILOTS = QStringLiteral("CREATE TABLE pilots ( "
            " pilot_id       INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            " lastname       TEXT    NOT NULL, "
            " firstname      TEXT, "
            " alias          TEXT, "
            " company        TEXT, "
            " employeeid     TEXT, "
            " phone          TEXT, "
            " email          TEXT "
//            " PRIMARY KEY(pilot_id AUTOINCREMENT)"
            ")"
                                                       );

const static auto CREATE_TABLE_TAILS = QStringLiteral("CREATE TABLE tails ("
            " tail_id        INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
            " registration   TEXT NOT NULL,"
            " company        TEXT,"
            " make           TEXT,"
            " model          TEXT,"
            " variant        TEXT,"
            " multipilot     INTEGER,"
            " multiengine    INTEGER,"
            " engineType     INTEGER,"
            " weightClass    INTEGER"
//            " PRIMARY KEY(tail_id AUTOINCREMENT)"
            ")"
                                                      );

const static auto CREATE_TABLE_FLIGHTS = QStringLiteral("CREATE TABLE flights ("
            " flight_id      INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
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
            " FOREIGN KEY(acft) REFERENCES tails(tail_id)   ON DELETE RESTRICT "
//            " PRIMARY KEY(flight_id    AUTOINCREMENT) "
        ")"
                                                        );

const static auto CREATE_TABLE_AIRPORTS = QStringLiteral("CREATE TABLE airports ( "
            " airport_id     INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            " icao           TEXT NOT NULL, "
            " iata           TEXT, "
            " name           TEXT, "
            " lat            REAL, "
            " long           REAL, "
            " country        TEXT, "
            " alt            INTEGER, "
            " utcoffset      INTEGER, "
            " tzolson        TEXT "
 //           " PRIMARY KEY(airport_id AUTOINCREMENT) "
            ")"
                                                         );

const static auto CREATE_TABLE_AIRCRAFT = QStringLiteral("CREATE TABLE aircraft ("
            " aircraft_id   INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
            " make          TEXT,"
            " model         TEXT,"
            " variant       TEXT,"
            " name          TEXT,"
            " iata          TEXT,"
            " icao          TEXT,"
            " multipilot    INTEGER,"
            " multiengine   INTEGER,"
            " engineType    INTEGER,"
            " weightClass   INTEGER"
//            " PRIMARY KEY(aircraft_id AUTOINCREMENT)"
            ")"
                                                         );

const static auto CREATE_TABLE_CHANGELOG = QStringLiteral("CREATE TABLE changelog ( "
            " revision   INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            " comment    TEXT, "
            " date       NUMERIC "
//            " PRIMARY KEY(revision) "
            ")"
                                                          );

const static auto CREATE_TABLE_CURRENCIES = QStringLiteral("CREATE TABLE currencies ( "
            " currency_id	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
            " description	TEXT, "
            " expiryDate     NUMERIC "
            ")"
                                                           );

const static auto CREATE_TABLE_SIMULATORS = QStringLiteral("CREATE TABLE simulators ( "
                                                       " session_id	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                                                       " date	NUMERIC NOT NULL, "
                                                       " totalTime	INTEGER NOT NULL, "
                                                       " deviceType	TEXT NOT NULL, "
                                                       " aircraftType	TEXT, "
                                                       " registration	TEXT, "
                                                       " remarks	TEXT "
                                                       ")"
                                                   );

// Statements for creation of views in the database
const static auto CREATE_VIEW_DEFAULT = QStringLiteral("CREATE VIEW viewDefault AS "
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

const static auto CREATE_VIEW_EASA = QStringLiteral("CREATE VIEW viewEASA AS "
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

const static auto CREATE_VIEW_TAILS = QStringLiteral("CREATE VIEW viewTails AS "
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

const static auto CREATE_VIEW_PILOTS = QStringLiteral("CREATE VIEW viewPilots AS "
        " SELECT "
        " pilot_id AS 'ID', "
        " lastname AS 'Last Name', "
        " firstname AS 'First Name', "
        " company AS 'Company' "
        " FROM pilots");

const static auto CREATE_VIEW_QCOMPLETER = QStringLiteral("CREATE VIEW viewQCompleter AS "
        " SELECT airport_id, icao, iata, tail_id, registration, pilot_id, "
        " lastname||', '||firstname AS 'pilot_name', alias "
        " FROM airports "
        " LEFT JOIN tails ON airports.airport_id = tails.tail_id "
        " LEFT JOIN pilots ON airports.airport_id = pilots.pilot_id");

const static auto CREATE_VIEW_TOTALS = QStringLiteral("CREATE VIEW viewTotals AS "
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

const static QStringList DATABASE_TABLES = {
    CREATE_TABLE_PILOTS,
    CREATE_TABLE_TAILS,
    CREATE_TABLE_FLIGHTS,
    CREATE_TABLE_AIRCRAFT,
    CREATE_TABLE_AIRPORTS,
    CREATE_TABLE_CURRENCIES,
    CREATE_TABLE_CHANGELOG,
    CREATE_TABLE_SIMULATORS,
};
const static QStringList DATABASE_VIEWS = {
    CREATE_VIEW_DEFAULT,
    CREATE_VIEW_EASA,
    CREATE_VIEW_TAILS,
    CREATE_VIEW_PILOTS,
    CREATE_VIEW_TOTALS,
    CREATE_VIEW_QCOMPLETER,
};

const static QStringList USER_TABLES = {
    QStringLiteral("flights"),
    QStringLiteral("pilots"),
    QStringLiteral("tails")
};
const static QStringList TEMPLATE_TABLES= {
    QStringLiteral("aircraft"),
    QStringLiteral("airports"),
    QStringLiteral("currencies"),
    QStringLiteral("changelog")
};

bool createDatabase()
{
    QSqlQuery q;
    QVector<QSqlError> errors;
    // Create Tables
    for (const auto &query_string : DATABASE_TABLES) {
        q.prepare(query_string);
        if (!q.exec())
            errors.append(q.lastError());
    }
    // Create Views
    for (const auto &query_string : DATABASE_VIEWS) {
        q.prepare(query_string);
        if (!q.exec())
            errors.append(q.lastError());
    }

    aDB->updateLayout();

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

bool commitData(const QJsonArray &json_arr, const QString &table_name)
{
    //aDB->updateLayout(); // needed?
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

bool importTemplateData(bool use_local_ressources)
{
    //QSqlQuery q;
    // reset template tables
    const auto table_names = aDB->getTemplateTableNames();
    for (const auto& table_name : table_names) {

        //clear table
        //q.prepare(QLatin1String("DELETE FROM ") + table_name);
        //if (!q.exec()) {
        //    DEB << "Error: " << q.lastError().text();
        //    return false;
        //}

        //Prepare data
        QJsonArray data_to_commit;
        QString error_message("Error importing data ");

        if (use_local_ressources) {
            data_to_commit = AJson::readFileToDoc(QLatin1String(":templates/database/templates/")
                                      + table_name + QLatin1String(".json")).array();
            error_message.append(QLatin1String(" (ressource) "));
        } else {
            data_to_commit = AJson::readFileToDoc(AStandardPaths::directory(
                                          AStandardPaths::Templates).absoluteFilePath(
                                          table_name + QLatin1String(".json"))).array();
            error_message.append(QLatin1String(" (downloaded) "));
        }

        // commit Data from Array
        if (!commitData(data_to_commit, table_name)) {
            LOG << error_message;
            return false;
        }
    } // for table_name
    return true;
}

bool resetUserData()
{
    QSqlQuery query;

    // clear user tables
    for (const auto& table : USER_TABLES) {
        query.prepare("DELETE FROM " + table);
        if (!query.exec()) {
            DEB << "Error: " << query.lastError().text();
        }
    }
    return true;
}

} // namespace aDbSetup
