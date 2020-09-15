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
 * \brief dbSetup::showDatabase Outputs database information to Console
 */
void dbSetup::showDatabase()
{
    QSqlQuery query;

    query.prepare("SELECT name FROM sqlite_master WHERE type='table'");
    query.exec();
    while (query.next()) {
        qDebug() << "Tables: " << query.value(0).toString();
    }

    query.prepare("SELECT name FROM sqlite_master WHERE type='view'");
    query.exec();
    while (query.next()) {
        qDebug() << "Views: " << query.value(0).toString();
    }
}

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
}

/*!
 * \brief dbSetup::createViews Create the required views for the database
 */
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
}

void dbSetup::csvtest()
{
    QStringList firstColumn;


    QFile f1("test.csv");
    f1.open(QIODevice::ReadOnly);
    QTextStream s1(&f1);
   // qDebug() << "Stream: " << s1.readAll();
    while (!s1.atEnd()){
      QString s=s1.readLine(); // reads line from file
      qDebug() << s;
      //firstColumn.append(s.split(",").first()); // appends first column to list, ',' is separator
      firstColumn.append(s.split(",")[1]);//second Column
    }
    qDebug() << "First Column: " << firstColumn;

    f1.close();
}

QVector<QStringList> dbSetup::importAirportsFromCSV()
{
    QStringList icao;
    QStringList iata;
    QStringList name;
    QStringList latitude;
    QStringList longitude;
    QStringList country;
    QStringList altitude;
    QStringList utcoffset;
    QStringList tzolson;



    QFile input("airports.csv");
    input.open(QIODevice::ReadOnly);
    QTextStream inputStream(&input);

    while (!inputStream.atEnd()) {
        QString line = inputStream.readLine();
        auto items = line.split(",");
        icao.append(items[0]);
        iata.append(items[1]);
        name.append(items[2]);
        latitude.append(items[3]);
        longitude.append(items[4]);
        country.append(items[5]);
        altitude.append(items[6]);
        utcoffset.append(items[7]);
        tzolson.append(items[8]);
    }

    QVector<QStringList> airportData = {
        icao,
        iata,
        name,
        latitude,
        longitude,
        country,
        altitude,
        utcoffset,
        tzolson
    };
    for(int i=0; i < airportData.length(); i++)
    {
        airportData[i].removeFirst();
    }
    //qDebug() << "Airport Data: " << airportData;
    return airportData;
}

void dbSetup::commitAirportData(QVector<QStringList> airportData)
{
    qDebug() << "Airport Data: " << airportData[0];

    QSqlQuery query;
    query.prepare("INSERT INTO airports ("
                  "icao, "
                  "iata, "
                  "name, "
                  "lat, "
                  "long, "
                  "country, "
                  "alt, "
                  "utcoffset, "
                  "tzolson"
                  ") "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
    //query.prepare("INSERT INTO airports (icao) VALUES (?)");
    query.addBindValue(airportData[0]);
    query.addBindValue(airportData[1]);
    query.addBindValue(airportData[2]);
    query.addBindValue(airportData[3]);
    query.addBindValue(airportData[4]);
    query.addBindValue(airportData[5]);
    query.addBindValue(airportData[6]);
    query.addBindValue(airportData[7]);
    query.addBindValue(airportData[8]);

    //if (!query.execBatch())
    //    qDebug() << query.lastError();
    auto start = std::chrono::high_resolution_clock::now(); // execution timer
    //code
    qDebug() << "Updating Airport Database...";
    query.execBatch();
    qDebug() << "Error: " << query.lastError();
    qDebug() << "Airport Database updated...";
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    qDebug() << "Time taken: " << duration.count() << " microseconds";


/*    for (int i = 0; i < airportData.length(); i++) {
        qDebug() << "List number " << i << ": " << airportData[i];
    }*/
}
