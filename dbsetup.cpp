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

/// db connection for debugging

void dbSetup::connect()
{
    const QString DRIVER("QSQLITE");

    if(QSqlDatabase::isDriverAvailable(DRIVER))
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(DRIVER);
        db.setDatabaseName("aaadebug.db");

        if(!db.open())
            qWarning() << "DatabaseConnect - ERROR: " << db.lastError().text();
    }
    else
        qWarning() << "DatabaseConnect - ERROR: no driver " << DRIVER << " available";
}

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

const QString createTableAirports = "CREATE TABLE airports( "
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


void dbSetup::createTables()
{
    QSqlQuery query;

    query.prepare(createTablePilots);
    query.exec();
    if(!query.isActive())
        qWarning() << "DatabaseInit - ERROR: " << query.lastError().text();

    query.prepare(createTableAircraft);
    query.exec();
    if(!query.isActive())
        qWarning() << "DatabaseInit - ERROR: " << query.lastError().text();

    query.prepare(createTableTails);
    query.exec();
    if(!query.isActive())
        qWarning() << "DatabaseInit - ERROR: " << query.lastError().text();

    query.prepare(createTableFlights);
    query.exec();
    if(!query.isActive())
        qWarning() << "DatabaseInit - ERROR: " << query.lastError().text();

    query.prepare(createTableExtras);
    query.exec();
    if(!query.isActive())
        qWarning() << "DatabaseInit - ERROR: " << query.lastError().text();

    query.prepare(createTableAirports);
    query.exec();
    if(!query.isActive())
        qWarning() << "DatabaseInit - ERROR: " << query.lastError().text();
    query.prepare(createTableScratchpad);
    query.exec();
    if(!query.isActive())
        qWarning() << "DatabaseInit - ERROR: " << query.lastError().text();
    query.prepare(createTableSettings);
    query.exec();
    if(!query.isActive())
        qWarning() << "DatabaseInit - ERROR: " << query.lastError().text();

    query.prepare("SELECT name FROM sqlite_master WHERE type='table'");
    query.exec();
    while (query.next()) {
        qDebug() << "Table: " << query.value(0).toString();
    }
}
