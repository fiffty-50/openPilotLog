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
 * Aircraft Database Related Functions
 */

    /*!
     * \brief RetreiveRegistration Looks up tail_id from Database
     * \param tail_ID Primary Key of tails database
     * \return Registration
     */
    static QString RetreiveRegistration(QString tail_ID)

    {
        QString acftRegistration("");

        QSqlQuery query;
        query.prepare("SELECT registration FROM tails WHERE tail_id == ?");
        query.addBindValue(tail_ID.toInt());
        query.exec();

        if(query.first());
        else
            qDebug() << ("No Aircraft with this ID found");
        query.previous();//To go back to index 0
        while (query.next()) {
            acftRegistration.append(query.value(0).toString());
        }

        return acftRegistration;
    }

    /*!
     * \brief newAcftGetString Looks up an aircraft Registration in the database
     * \param searchstring
     * \return Registration, make, model and variant
     */
    static QStringList newAcftGetString(QString searchstring)
    {
        QStringList result;
        if(searchstring.length()<2){return result;}
        QSqlQuery query;
        query.prepare("SELECT registration, make, model, variant "
                      "FROM aircraft "
                      "INNER JOIN tails on tails.aircraft_ID = aircraft.aircraft_id "
                      "WHERE tails.registration LIKE ?");
        searchstring.append("%"); searchstring.prepend("%");
        query.addBindValue(searchstring);
        query.exec();

        while(query.next())
        {
            result.append(query.value(0).toString() + " (" + query.value(1).toString() + "-" + query.value(2).toString() + "-" + query.value(3).toString() + ")");
        }
        qDebug() << "newAcftGetString: " << result.length() << result;
        return result;

    }

    static QString newAcftGetId(QString registration)
    {
        QString result;
        QSqlQuery query;
        query.prepare("SELECT tail_id "
                      "FROM tails "
                      "WHERE registration LIKE ?");
        registration.prepend("%"); registration.append("%");
        query.addBindValue(registration);
        query.exec();

        while(query.next())
        {
            result.append(query.value(0).toString());
        }
        qDebug() << "newAcftGetId: " << result;
        return result;

    }

    static QVector<QString> RetreiveAircraftTypeFromReg(QString searchstring)
    /*
     * Searches the tails Database and returns the aircraft Type.
     */
    {

        QSqlQuery query;
        query.prepare("SELECT Name, iata, registration, tail_id " //"SELECT Registration, Name, icao, iata "
                      "FROM aircraft "
                      "INNER JOIN tails on tails.aircraft_ID = aircraft.aircraft_id "
                      "WHERE tails.registration LIKE ?");
        // Returns Registration/Name/icao/iata
        searchstring.prepend("%");
        searchstring.append("%");
        query.addBindValue(searchstring);
        query.exec();

        QVector<QString> result;

        if(query.first())
        {
            QString acType = query.value(0).toString();
            QString iataCode = query.value(1).toString();
            QString registration = query.value(2).toString();
            QString tail_id = query.value(3).toString();
            //QString formatted = acType + " [ " + registration + " | " + iataCode + " ]";
            //qDebug() << formatted;
            result.append(registration); result.append(acType);
            result.append(iataCode); result.append(tail_id);
            return result;
        }else
        {
            return  result; // empty vector
        }


    }

    static QStringList RetreiveAircraftMake(QString searchstring)
    {
        QStringList result;
        QSqlQuery query;
        query.prepare("SELECT make from aircraft WHERE make LIKE ?");
        searchstring.prepend("%"); searchstring.append("%");
        query.addBindValue(searchstring);
        query.exec();


        while(query.next())
        {
            result.append(query.value(0).toString());
        }
        qDebug() << "db::RetreiveAircraftMake says... Result:" << result;
        return result;
    }

    static QStringList RetreiveAircraftModel(QString make, QString searchstring)
    {
        QStringList result;
        QSqlQuery query;
        query.prepare("SELECT model FROM aircraft WHERE make = ? AND model LIKE ?");
        query.addBindValue(make);
        searchstring.prepend("%"); searchstring.append("%");
        query.addBindValue(searchstring);
        query.exec();

        while(query.next())
        {
            result.append(query.value(0).toString());
            qDebug() << "db::RetreiveAircraftModel says... Result:" << result;
        }

        return result;
    }

    static QStringList RetreiveAircraftVariant(QString make, QString model, QString searchstring)
    {
        QStringList result;
        QSqlQuery query;
        query.prepare("SELECT variant from aircraft WHERE make = ? AND model = ? AND variant LIKE ?");
        query.addBindValue(make);
        query.addBindValue(model);
        searchstring.prepend("%"); searchstring.append("%");
        query.addBindValue(searchstring);
        query.exec();

        while(query.next())
        {
            result.append(query.value(0).toString());
            qDebug() << "db::RetreiveAircraftVariant says... Result:" << result;
        }

        return result;
    }

    static QString RetreiveAircraftIdFromMakeModelVariant(QString make, QString model, QString variant)
    {
        QString result;
        QSqlQuery query;
        query.prepare("SELECT aircraft_id FROM aircraft WHERE make = ? AND model = ? AND variant = ?");
        query.addBindValue(make);
        query.addBindValue(model);
        query.addBindValue(variant);
        query.exec();

        if(query.first())
        {
            result.append(query.value(0).toString());
            qDebug() << "db::RetreiveAircraftIdFromMakeModelVariant: Aircraft found! ID# " << result;
            return result;
        }else
        {
            result = result.left(result.length()-1);
            result.append("0");
            qDebug() << "db::RetreiveAircraftIdFromMakeModelVariant: ERROR - no AircraftId found.";
            return  result;
        }
    }

    static bool CommitTailToDb(QString registration, QString aircraft_id, QString company)
    {
        QSqlQuery commit;
        commit.prepare("INSERT INTO tails (registration, aircraft_id, company) VALUES (?,?,?)");
        commit.addBindValue(registration);
        commit.addBindValue(aircraft_id);
        commit.addBindValue(company);
        commit.exec();
        QString error = commit.lastError().text();
        if(error.length() < 0)
        {
            qDebug() << "db::CommitAircraftToDb:: SQL error:" << error;
            return false;
        }else
        {
            return true;
        }
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
            QString acft = db::RetreiveRegistration(query.value(8).toString());
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
