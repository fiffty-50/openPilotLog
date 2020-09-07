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
#include "dbsetup.h"

/// dummy db connection for debugging

void dbSetup::connect()
{
    const QString DRIVER("QSQLITE");

    if(QSqlDatabase::isDriverAvailable(DRIVER))
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(DRIVER);
        db.setDatabaseName("debug.db");

        if(!db.open())
            qWarning() << "DatabaseConnect - ERROR: " << db.lastError().text();
    }
    else
        qWarning() << "DatabaseConnect - ERROR: no driver " << DRIVER << " available";
}

// Pragmas for creation of database table
const QString createTablePilots = "CREATE TABLE pilots ( "
                            "pilot_id       INTEGER, "
                            "picfirstname	TEXT, "
                            "piclastname	TEXT NOT NULL, "
                            "alias          TEXT, "
                            "phone          TEXT, "
                            "email          TEXT, "
                            "rostername     TEXT, "
                            "PRIMARY KEY(pilot_id) "
                            ")";

const QString createTableAircraft = "CREATE TABLE aircraft ( "
                             "aircraft_id	integer, "
                             "make		TEXT, "
                             "model		TEXT, "
                             "variant	text, "
                             "name		TEXT, "
                             "iata		TEXT, "
                             "icao		TEXT, "
                             "singlepilot	INTEGER, "
                             "multipilot	INTEGER, "
                             "singleengine	INTEGER, "
                             "multiengine	INTEGER, "
                             "turboprop	INTEGER, "
                             "jet		INTEGER, "
                             "heavy		INTEGER, "
                             "PRIMARY KEY(aircraft_id) "
                             ") ";

const QString createTableTails = "CREATE TABLE tails ( "
                             "tail_id       INTEGER, "
                             "registration	TEXT NOT NULL, "
                             "aircraft_id	INTEGER NOT NULL, "
                             "company       TEXT, "
                             "PRIMARY KEY(tail_id), "
                             "FOREIGN KEY(aircraft_id) REFERENCES aircraft(aircraft_id) "
                             ")";

const QString createTableFlights = "CREATE TABLE flights  ( "
                             "id	INTEGER, "
                             "doft	NUMERIC NOT NULL, "
                             "dept	TEXT    NOT NULL, "
                             "tofb	INTEGER NOT NULL, "
                             "dest	TEXT    NOT NULL, "
                             "tonb	INTEGER NOT NULL, "
                             "tblk	INTEGER NOT NULL, "
                             "pic	INTEGER, "
                             "acft	INTEGER, "
                             "PRIMARY KEY(id), "
                             "FOREIGN KEY(pic)  REFERENCES pilots(pilot_id), "
                             "FOREIGN KEY(acft) REFERENCES tails(tail_id) "
                             ")";
//extras table might eventually be merged into flights table.
const QString createTableExtras = "CREATE TABLE extras ( "
                            "extras_id      INTEGER NOT NULL, "
                            "PilotFlying	INTEGER, "
                            "TOday          INTEGER, "
                            "TOnight        INTEGER, "
                            "LDGday         INTEGER, "
                            "LDGnight       INTEGER, "
                            "autoland       INTEGER, "
                            "tSPSE          INTEGER, "
                            "tSPME          INTEGER, "
                            "tMPME          INTEGER, "
                            "tNight         INTEGER, "
                            "tIFR           INTEGER, "
                            "tPIC           INTEGER, "
                            "tSIC           INTEGER, "
                            "tDual          INTEGER, "
                            "tInstructor	INTEGER, "
                            "tSIM           INTEGER, "
                            "ApproachType	TEXT, "
                            "FlightNumber	TEXT, "
                            "Remarks        TEXT, "
                            "PRIMARY KEY(extras_id) "
                            ")";

const QString createTableAirports = "CREATE TABLE airports ( "
                            "airport_id INTEGER primary key, "
                            "icao       TEXT NOT NULL, "
                            "iata       TEXT, "
                            "name       TEXT, "
                            "lat        REAL, "
                            "long       REAL, "
                            "country    TEXT,  "
                            "alt        INTEGER, "
                            "utcoffset  INTEGER,  "
                            "tzolson    TEXT "
                            ")";
const QString createTableScratchpad = "CREATE TABLE scratchpad ( "
                            "id     INTEGER, "
                            "doft	NUMERIC, "
                            "dept	TEXT, "
                            "tofb	INTEGER, "
                            "dest	TEXT, "
                            "tonb	INTEGER, "
                            "tblk	INTEGER, "
                            "pic	INTEGER, "
                            "acft	INTEGER, "
                            "PRIMARY KEY(id) "
                            ") ";

const QString createTableSettings = "CREATE TABLE settings ( "
                             "setting_id	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
                             "setting       TEXT, "
                             "description	TEXT "
                             ")";

// Pragmas for creation of views in the database

const QString createViewQCompleterView = "CREATE VIEW QCompleterView AS "
                              "SELECT airport_id, icao, iata, "
                              "tail_id, registration, "
                              "pilot_id, "
                              "piclastname||', '||picfirstname AS 'pilot_name', "
                              "alias "
                              "FROM airports "
                              "LEFT JOIN tails ON airports.airport_id = tails.tail_id "
                              "LEFT JOIN pilots ON airports.airport_id = pilots.pilot_id";

const QString createViewAircraftList = "CREATE VIEW AircraftListView AS "
                              "SELECT make AS 'Make', "
                              "model AS 'Model', "
                              "variant AS 'Variant' "
                              "FROM aircraft "
                              "WHERE variant IS NOT NULL";

const QString createViewLogbook = "CREATE VIEW Logbook AS "
                              "SELECT id, doft as 'Date', dept AS 'Dept', "
                              "printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time', "
                              "dest AS 'Dest', "
                              "printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time ', "
                              "printf('%02d',(tblk/60))||':'||printf('%02d',(tblk%60)) AS 'Total', "
                              "piclastname||', '||substr(picfirstname,1,1)||'.' AS 'Name PIC', "
                              "make||' '||model||'-'||variant AS 'Type', Registration, "
                              "FlightNumber AS 'Flight #', Remarks "
                              "FROM flights "
                              "INNER JOIN pilots on flights.pic = pilots.pilot_id "
                              "INNER JOIN tails on flights.acft = tails.tail_id "
                              "INNER JOIN aircraft on tails.aircraft_id = aircraft.aircraft_id "
                              "INNER JOIN extras on extras.extras_id = flights.id "
                              "ORDER BY date DESC ";

//Displays Single Engine, Multi Engine and Multi Pilot Time
const QString createViewTimes = "CREATE VIEW ViewPilotTimes AS "
                              "SELECT id, "
                              "tblk*singlepilot*singleengine AS 'SPSE', "
                              "tblk*singlepilot*multiengine AS 'SPME', "
                              "tblk*multipilot AS 'MP' "
                              "FROM flights "
                              "INNER JOIN tails on flights.acft = tails.tail_id "
                              "INNER JOIN aircraft on tails.aircraft_id = aircraft.aircraft_id ";
QStringList tables = {
    createTablePilots,
    createTableAircraft,
    createTableTails,
    createTableFlights,
    createTableExtras,
    createTableScratchpad,
    createTableAirports,
    createTableSettings
};
QStringList views = {
    createViewQCompleterView,
    createViewAircraftList,
    createViewLogbook,
    createViewTimes
};

/*!
 * \brief dbSetup::createTables Create the required tables for the database
 */
void dbSetup::createTables()
{
    QSqlQuery query;

    for(int i = 0; i<tables.length() ; i++) {
        query.prepare(tables[i]);
        query.exec();
        if(!query.isActive()) {
            qWarning() << "DatabaseInit - ERROR: " << query.lastError().text();
        }else
            qDebug() << "Adding table " << tables[i].section(QLatin1Char(' '),2,2);
    }
    //verify tables are created
    query.prepare("SELECT name FROM sqlite_master WHERE type='table'");
    query.exec();
    while (query.next()) {
        qDebug() << "Table: " << query.value(0).toString();
    }
}

void dbSetup::createViews()
{
    QSqlQuery query;

    for(int i = 0; i<views.length() ; i++) {
        query.prepare(views[i]);
        query.exec();
        if(!query.isActive()){
            qWarning() << "DatabaseInit - ERROR: " << query.lastError().text();
        }else{
            qDebug() << "Adding View " << views[i].section(QLatin1Char(' '),2,2);
        }
    }
    //verify views are created
    query.prepare("SELECT name FROM sqlite_master WHERE type='view'");
    query.exec();
    while (query.next()) {
        qDebug() << "View: " << query.value(0).toString();
    }
}
