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
#include <QCoreApplication>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include "calc.h"
#include "dbpilots.h"
#include "dbaircraft.h"
#include <chrono>
#include <QRandomGenerator>
#include <QStandardPaths>


class db
{
public:
    static void connect()
    {
        const QString DRIVER("QSQLITE");

        if(QSqlDatabase::isDriverAvailable(DRIVER))
        {
            QSqlDatabase db = QSqlDatabase::addDatabase(DRIVER);

            //QString pathtodb = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
            //db.setDatabaseName(pathtodb+"/logbook.db");
            //qDebug() << "Database: " << pathtodb+"/logbook.db";
            db.setDatabaseName("logbook.db");


            if(!db.open())
                qWarning() << "MainWindow::DatabaseConnect - ERROR: " << db.lastError().text();
        }
        else
            qWarning() << "MainWindow::DatabaseConnect - ERROR: no driver " << DRIVER << " available";
    }
    static void initexample()
    {
        QSqlQuery query("CREATE TABLE flights (id INTEGER PRIMARY KEY, date NUMERIC)");

        if(!query.isActive())
            qWarning() << "MainWindow::DatabaseInit - ERROR: " << query.lastError().text();

    }

    static void queryexamplenamedbinding()
    {
        QSqlQuery query;
        //query.prepare("SELECT * FROM people");
        //query.prepare("SELECT * FROM people WHERE name LIKE 'Linus' OR id = :id");
        query.prepare("SELECT * from people WHERE name LIKE :name");
        query.bindValue(":name", "%Linus%");
        query.bindValue(":id",2);
        query.exec();
        /*
         * QSqlQuery provides access to the result set one record at a time. After the call to exec(),
         * QSqlQuery's internal pointer is located one position before the first record.
         * We must call QSqlQuery::next() once to advance to the first record, then next() again
         * repeatedly to access the other records, until it returns false. Here's a typical loop that
         * iterates over all the records in order:
         * After a SELECT query is executed we have to browse the records (result rows) returned to access
         * the data. In this case we try to retrieve the first record calling the function first which
         * returns true when the query has been successful and false otherwise.
         */
        if(query.first());
        else
            qDebug() << ("No entry found");
        query.previous();//To go back to index 0
        while (query.next()) {
            QString name = query.value(1).toString();
            int id = query.value(0).toInt();
            qDebug() << name << id;
        }
        /*
         *The QSqlQuery::value() function returns the value of a field in the current record. Fields are
         * specified as zero-based indexes. QSqlQuery::value() returns a QVariant, a type that can hold
         * various C++ and core Qt data types such as int, QString, and QByteArray. The different database
         * types are automatically mapped into the closest Qt equivalent. In the code snippet, we call
         * QVariant::toString() and QVariant::toInt() to convert variants to QString and int.
         */
    }





/*
 *  Obsolete Functions
 */
    /*!
     * \brief SelectFlightDate Retreives Flights from the database currently not in use.
     * \param doft Date of flight for filtering result set. "ALL" means no filter.
     * \return Flight(s) for selected date.
     */
    static QVector<QString> SelectFlightDate(QString doft)
    {
        QSqlQuery query;
        if (doft == "ALL") // Special Selector
        {
            query.prepare("SELECT * FROM flights ORDER BY doft DESC, tofb ASC");
            qDebug() << "All flights selected";
        }else
        {
            query.prepare("SELECT * FROM flights WHERE doft = ? ORDER BY tofb ASC");
            query.addBindValue(doft);
            qDebug() << "Searching flights for " << doft;
        }

        query.exec();

        if(query.first());
        else
        {
            qDebug() << ("No flight with this date found");
            QVector<QString> flight; //return empty
            return flight;
        }

        query.previous();// To go back to index 0
        query.last(); // this can be very slow, used to determine query size since .size is not supported by sqlite
        int numRows = query.at() + 1; // Number of rows (flights) in the query
        query.first();
        query.previous();// Go back to index 0

        QVector<QString> flight(numRows * 9); // Every flight has 9 fields in the database
        int index = 0; // counter for output vector

        while (query.next()) {
            QString id = query.value(0).toString();
            QString doft = query.value(1).toString();
            QString dept = query.value(2).toString();
            QString tofb = calc::minutes_to_string((query.value(3).toString()));
            QString dest = query.value(4).toString();
            QString tonb = calc::minutes_to_string((query.value(5).toString()));
            QString tblk = calc::minutes_to_string((query.value(6).toString()));
            QString pic = dbPilots::retreivePilotNameFromID(query.value(7).toString());
            QString acft = dbAircraft::retreiveRegistration(query.value(8).toString());
            //qDebug() << id << doft << dept << tofb << dest << tonb << tblk << pic << acft << endl;
            flight[index] = id;
            ++index;
            flight[index] = doft;
            ++index;
            flight[index] = dept;
            ++index;
            flight[index] = tofb;
            ++index;
            flight[index] = dest;
            ++index;
            flight[index] = tonb;
            ++index;
            flight[index] = tblk;
            ++index;
            flight[index] = pic;
            ++index;
            flight[index] = acft;
            ++index;
        }
        return flight;
    }
};
