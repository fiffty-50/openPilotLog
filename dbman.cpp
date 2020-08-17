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
            //db.setDatabaseName(pathtodb+"/flog.db");
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
        QSqlQuery query("CREATE TABLE people (id INTEGER PRIMARY KEY, name TEXT)");

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
     *
     *
     * Flights Database Related Functions
     *
     *
     */


    static QVector<QString> SelectFlightDate(QString doft)
    {
        QSqlQuery query;
        if (doft == "ALL")
        {
            query.prepare("SELECT * FROM flights ORDER BY doft DESC, tofb ASC");
            qDebug() << "All flights selected";
        }else
        {
            query.prepare("SELECT * FROM flights WHERE doft = ? ORDER BY tofb ASC");
            query.addBindValue(doft);
            qDebug() << "Searching flights for " << doft;
        }
        //query.prepare("SELECT * FROM flights WHERE doft = ?");

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
            QString pic = db::RetreivePilotNameFromID(query.value(7).toString());
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
        //QString minutes = calc::minutes_to_string(flight[3]);
        //flight.replace(3, minutes);
        //qDebug() << "Length of flight vector " << flight.size();
        return flight;

    }
    static QVector<QString> SelectFlightById(QString flight_id)
    {
        QSqlQuery query;
        query.prepare("SELECT * FROM flights WHERE id = ?");
        query.addBindValue(flight_id);
        query.exec();

        if(query.first());
        else
        {
            qDebug() << "db::SelectFlightById - No Flight with this ID found";
            QVector<QString> flight; //return empty
            return flight;
        }

        QVector<QString> flight;
        flight.append(query.value(0).toString());
        flight.append(query.value(1).toString());
        flight.append(query.value(2).toString());
        flight.append(query.value(3).toString());
        flight.append(query.value(4).toString());
        flight.append(query.value(5).toString());
        flight.append(query.value(6).toString());
        flight.append(query.value(7).toString());
        flight.append(query.value(8).toString());
        /*
        flight.append(calc::minutes_to_string((query.value(3).toString())));
        flight.append(query.value(4).toString());
        flight.append(calc::minutes_to_string((query.value(5).toString())));
        flight.append(calc::minutes_to_string((query.value(6).toString())));
        flight.append(db::RetreivePilotNameFromID(query.value(7).toString()));
        flight.append(db::RetreiveRegistration(query.value(8).toString()));
        */


        qDebug() << "db::SelectFlightById - retreived flight: " << flight;
        return flight;

    }
    static QVector<QString> CreateFlightVectorFromInput(QString doft, QString dept, QTime tofb, QString dest, QTime tonb, QTime tblk, QString pic, QString acft)
    {

        QVector<QString> flight;
        flight.insert(0, ""); // ID, created as primary key during commit
        flight.insert(1, doft);
        flight.insert(2, dept);
        flight.insert(3, QString::number(calc::time_to_minutes(tofb)));
        flight.insert(4, dest);
        flight.insert(5, QString::number(calc::time_to_minutes(tonb)));
        flight.insert(6, QString::number(calc::time_to_minutes(tblk)));
        flight.insert(7, pic); // lookup and matching tbd
        flight.insert(8, acft);// lookup and matching tbd
        //qDebug() << flight;
        return flight;
    }
    static int CommitFlight(QVector<QString> flight)// flight vector shall always have length 9
    {
        QSqlQuery query;
        query.prepare("INSERT INTO flights (doft, dept, tofb, dest, tonb, tblk, pic, acft) "
                      "VALUES (:doft, :dept, :tofb, :dest, :tonb, :tblk, :pic, :acft)");
        //flight[0] is primary key, not required for commit
        query.bindValue(":doft", flight[1]); //string
        query.bindValue(":dept", flight[2]);
        query.bindValue(":tofb", flight[3].toInt()); //int
        query.bindValue(":dest", flight[4]);
        query.bindValue(":tonb", flight[5].toInt());
        query.bindValue(":tblk", flight[6].toInt());
        query.bindValue(":pic", flight[7].toInt());
        query.bindValue(":acft", flight[8].toInt());
        query.exec();
        qDebug() << "Error message for commiting flight: " << query.lastError().text();

        QSqlQuery query2;
        query2.prepare("INSERT INTO extras DEFAULT VALUES");
        query2.exec();

        qDebug() << "Creating extras entry" << query2.lastError().text();
        return 0;
    }

    static int CommitToScratchpad(QVector<QString> flight)// to store input mask
    {
        //qDebug() << "Saving invalid flight to scratchpad";
        QSqlQuery query;
        query.prepare("INSERT INTO scratchpad (doft, dept, tofb, dest, tonb, tblk, pic, acft) "
                      "VALUES (:doft, :dept, :tofb, :dest, :tonb, :tblk, :pic, :acft)");
        //flight[0] is primary key, not required for commit
        query.bindValue(":doft", flight[1]); //string
        query.bindValue(":dept", flight[2]);
        query.bindValue(":tofb", flight[3].toInt()); //int
        query.bindValue(":dest", flight[4]);
        query.bindValue(":tonb", flight[5].toInt());
        query.bindValue(":tblk", flight[6].toInt());
        query.bindValue(":pic", flight[7].toInt());
        query.bindValue(":acft", flight[8].toInt());
        query.exec();
        qDebug() << query.lastError().text();
        return 0;
    }
    static bool CheckScratchpad() // see if scratchpad is empty
    {
        //qDebug() << "Checking if scratchpad contains data";
        QSqlQuery query;
        query.prepare("SELECT * FROM scratchpad");
        query.exec();

        if(query.first())
        {
            //qDebug() << "Scratchpad contains data";
            return 1;
        }
        else
        {
            //qDebug() << ("Scratchpad contains NO data");
            return 0;
        }
    }
    static QVector<QString> RetreiveScratchpad()
    {
        //qDebug() << "Retreiving invalid flight from scratchpad";
        QSqlQuery query;
        query.prepare("SELECT * FROM scratchpad");
        query.exec();

        if(query.first());
        else
        {
            //qDebug() << ("scratchpad empty");
            QVector<QString> flight; //return empty
            return flight;
        }

        query.previous();// To go back to index 0
        //query.last(); // this can be very slow, used to determine query size since .size is not supported by sqlite
        //int numRows = query.at() + 1; // Number of rows (flights) in the query
        //query.first();
        //query.previous();// Go back to index 0

        //QVector<QString> flight(numRows * 9); // Every flight has 9 fields in the database
        QVector<QString> flight(9);
        int index = 0; // counter for output vector

        while (query.next()) {
            QString id = query.value(0).toString();
            QString doft = query.value(1).toString();
            QString dept = query.value(2).toString();
            QString tofb = calc::minutes_to_string((query.value(3).toString()));
            QString dest = query.value(4).toString();
            QString tonb = calc::minutes_to_string((query.value(5).toString()));
            QString tblk = calc::minutes_to_string((query.value(6).toString()));
            QString pic = query.value(7).toString();
            QString acft = query.value(8).toString();
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

        ClearScratchpad();
        return flight;
    }
    static void ClearScratchpad()
    {
        qDebug() << "Deleting scratchpad";
        QSqlQuery query;
        query.prepare("DELETE FROM scratchpad;");
        query.exec();
    }

    static bool DeleteFlightById(QString flight_id)
    {
        QSqlQuery query;
        query.prepare("DELETE FROM flights WHERE id = ?");
        query.addBindValue(flight_id);
        query.exec();
        QString error = query.lastError().text();
        qDebug() << "db::DeleteFlightById: Removing flight with ID#: " << flight_id << "Query Error: " << error;
        if(error.length() > 0)
        {
            return false;
        }else
        {
            return true;
        }

    }

    /*
     *
     *
     * Pilots Database Related Functions
     *
     *
     */

    static QString RetreivePilotNameFromID(QString pilotID)
    /* Looks up the pilot ID in the Database and returns the name as a string
     * unless the pilot in command is the logbook owner.
     */
    {
        QString pilotName("");
        if (pilotID == "1")
        {
            pilotName = "self";
            return pilotName;
        }
        QSqlQuery query;
        query.prepare("SELECT piclastname, picfirstname, alias FROM pilots WHERE pilot_id == ?");
        query.addBindValue(pilotID.toInt());
        query.exec();

        //if(query.first());
        //else
        //  qDebug() << ("No Pilot with this ID found");
        //query.previous();//To go back to index 0
        while (query.next()) {
            pilotName.append(query.value(0).toString());
            pilotName.append(", ");
            pilotName.append(query.value(1).toString());//.left(1));
            //pilotName.append(".");
        }

        if(pilotName.length() == 0)
        {
            qDebug() << ("No Pilot with this ID found");
        }
        return pilotName;
    }

    static QString RetreivePilotIdFromString(QString lastname, QString firstname)
    {
        QSqlQuery query;
        query.prepare("SELECT pilot_id from pilots "
                      "WHERE piclastname = ? AND picfirstname LIKE ?");
        query.addBindValue(lastname);
        firstname.prepend("%"); firstname.append("%");
        query.addBindValue(firstname);
        query.exec();

        QString id;
        if(query.first()){id.append(query.value(0).toString());}

        return id;

    }

    static QStringList RetreivePilotNameFromString(QString searchstring)
    /* Searches the pilot Name in the Database and returns the name as a vector of results
     * unless the pilot in command is the logbook owner.
     */
    {
        QString firstname = searchstring; //To Do: Two control paths, one for single word, query as before with only searchstring
        QString lastname = searchstring;  // second control path with comma, lastname like AND firstname like
        if(searchstring.contains(QLatin1Char(',')))
        {
            QStringList namelist = searchstring.split(QLatin1Char(','));
            QString lastname = namelist[0].trimmed();
            lastname = lastname.toLower();
            lastname[0] = lastname[0].toUpper();
            lastname.prepend("%"), lastname.append("%");

            QString firstname = namelist[1].trimmed();
            if(firstname.length()>1)
            {
                firstname = firstname.toLower();
                firstname[0] = firstname[0].toUpper();
                firstname.prepend("%"), firstname.append("%");
            }
            qDebug() << "db::RetreivePilotNameFromString: first last after comma";
            qDebug() << firstname << lastname;
        }
        QSqlQuery query;
        query.prepare("SELECT piclastname, picfirstname, alias "
                      "FROM pilots WHERE  "
                      "picfirstname LIKE ? OR piclastname LIKE ? OR alias LIKE ?");
        searchstring.prepend("%");
        searchstring.append("%");
        query.addBindValue(firstname);
        query.addBindValue(lastname);
        query.addBindValue(searchstring);
        query.exec();


        QStringList result;
        while (query.next()) {
            QString piclastname = query.value(0).toString();
            QString picfirstname = query.value(1).toString();
            QString alias = query.value(2).toString();
            QString name = piclastname + ", " + picfirstname;
            result.append(name);
        }
        qDebug() << "db::RetreivePilotNameFromString Result: "  << result;
        //qDebug() << query.lastError();


        if(result.size() == 0)
        {
            qDebug() << ("db::RetreivePilotNameFromString: No Pilot found");
            return result;
        }


        return result;
    }
    static QStringList newPicGetString(QString searchstring)
    /*
     * This function is returning a QStringList for the QCompleter in the NewFlight::newPic line edit.
     * A regular expression limits the input possibilities to only characters, followed by an optional ',' and 1 whitespace, e.g.:
     * Miller, Jim ->valid / Miller,  Jim -> invalid / Miller,, Jim -> invalid
     * Miller Jim -> valid / Miller  Jim ->invalid
     * Jim Miller-> valid
     */
    {
        QStringList result;
        QStringList searchlist;

        if(searchstring == "self")
        {
            result.append("self");
            qDebug() << "Pilot is self";
            return result;
        }



        //Fall 1) Nachname, Vorname
        if(searchstring.contains(QLatin1Char(',')))
        {
            QStringList namelist = searchstring.split(QLatin1Char(','));

            QString name1 = namelist[0].trimmed();
            name1 = name1.toLower();
            name1[0] = name1[0].toUpper();
            searchlist.append(name1);

            if(namelist[1].length() > 1)
            {
                QString name2 = namelist[1].trimmed();
                name2 = name2.toLower();
                name2[0] = name2[0].toUpper();
                searchlist.append(name2);
            }



        }
        //Fall 2) Vorname Nachname
        if(searchstring.contains(" ") && !searchstring.contains(QLatin1Char(',')))
        {
            QStringList namelist = searchstring.split(" ");

            QString name1 = namelist[0].trimmed();
            name1 = name1.toLower();
            name1[0] = name1[0].toUpper();
            searchlist.append(name1);

            if(namelist[1].length() > 1) //To avoid index out of range if the searchstring is one word followed by only one whitespace
            {
                QString name2 = namelist[1].trimmed();
                name2 = name2.toLower();
                name2[0] = name2[0].toUpper();
                searchlist.append(name2);

            }

        }
        //Fall 3) Name
        if(!searchstring.contains(" ") && !searchstring.contains(QLatin1Char(',')))
        {

            QString name1 = searchstring.toLower();
            name1[0] = name1[0].toUpper();
            searchlist.append(name1);
        }

        if(searchlist.length() == 1)
        {
            QSqlQuery query;
            query.prepare("SELECT piclastname, picfirstname FROM pilots "
                          "WHERE piclastname LIKE ?");
            query.addBindValue(searchlist[0] + '%');
            query.exec();



            while(query.next())
            {
                result.append(query.value(0).toString() + ", " + query.value(1).toString());
            }

            QSqlQuery query2;
            query2.prepare("SELECT piclastname, picfirstname FROM pilots "
                           "WHERE picfirstname LIKE ?");
            query2.addBindValue(searchlist[0] + '%');
            query2.exec();

            while(query2.next())
            {
                result.append(query2.value(0).toString() + ", " + query2.value(1).toString());
            }

        }else
        {
            QSqlQuery query;
            query.prepare("SELECT piclastname, picfirstname FROM pilots "
                          "WHERE piclastname LIKE ? AND picfirstname LIKE ?");
            query.addBindValue(searchlist[0] + '%');
            query.addBindValue(searchlist[1] + '%');
            query.exec();

            while(query.next())
            {
                result.append(query.value(0).toString() + ", " + query.value(1).toString());
            }

            QSqlQuery query2;
            query2.prepare("SELECT piclastname, picfirstname FROM pilots "
                          "WHERE picfirstname LIKE ? AND piclastname LIKE ?");
            query2.addBindValue(searchlist[0] + '%');
            query2.addBindValue(searchlist[1] + '%');
            query2.exec();

            while(query2.next())
            {
                result.append(query2.value(0).toString() + ", " + query2.value(1).toString());
            }

        }


        qDebug() << "db::newPic Result" << result.length() << result;
        if(result.length() == 0)
        {
            //try first name search
            qDebug() << "No Pilot with this last name found. trying first name search.";
            return result;
        }else
        {
            return result;
        }

    }
    static QString newPicGetId(QString name)
    {
        QString result;

        QStringList nameparts = name.split(QLatin1Char(','));
        QString lastname = nameparts[0].trimmed();
        lastname = lastname.toLower(); lastname[0] = lastname[0].toUpper();
        QString firstname = nameparts[1].trimmed();
        firstname = firstname.toLower(); firstname[0] = firstname[0].toUpper();
        firstname.prepend("%"); firstname.append("%");

        QSqlQuery query;
        query.prepare("SELECT pilot_id FROM pilots "
                      "WHERE piclastname = ? AND picfirstname LIKE ?");
        query.addBindValue(lastname);
        query.addBindValue(firstname);
        query.exec();

        while (query.next())
        {
            result.append(query.value(0).toString());
        }

        qDebug() << "newPicGetId: result = " << result;
        return result;
    }
/*
 *
 *
 * Airport Database Related Functions
 *
 *
 */
    static QString RetreiveAirportNameFromIcaoOrIata(QString identifier)
    /*
     * 'EDDF' gets looked up and 'Frankfurt International Airport' returned
     *
     */
    {
        QString result = "";
        QSqlQuery query;
        query.prepare("SELECT name "
                      "FROM airports WHERE icao LIKE ? OR iata LIKE ?");
        identifier.append("%");
        identifier.prepend("%");
        query.addBindValue(identifier);
        query.addBindValue(identifier);
        query.exec();
        if(query.first())
        {
            result.append(query.value(0).toString());
            return result;
        }else
        {
            result = result.left(result.length()-1);
            result.append("No matching airport found.");
            return  result;
        }
    }

    static QString RetreiveAirportIdFromIcao(QString identifier)
    {
        QString result;
        QSqlQuery query;
        query.prepare("SELECT airport_id FROM airports WHERE icao = ?");
        query.addBindValue(identifier);
        query.exec();

        while(query.next())
        {
            result.append(query.value(0).toString());
            //qDebug() << "db::RetreiveAirportIdFromIcao says Airport found! #" << result;
        }

        return result;
    }
    static QStringList CompleteIcaoOrIata(QString icaoStub)
    {
        QStringList result;
        QSqlQuery query;
        query.prepare("SELECT icao FROM airports WHERE icao LIKE ? OR iata LIKE ?");
        icaoStub.prepend("%"); icaoStub.append("%");
        query.addBindValue(icaoStub);
        query.addBindValue(icaoStub);
        query.exec();

        while(query.next())
        {
            result.append(query.value(0).toString());
            qDebug() << "db::CompleteIcaoOrIata says... Result:" << result;
        }

        return result;
    }

    static bool CheckICAOValid(QString identifier)
    // Verifies if a user input airport exists in the database
    {
        if(identifier.length() == 4)
        {
            QString check = RetreiveAirportIdFromIcao(identifier);
            if(check.length() > 0)
            {
                //qDebug() << "db::CheckICAOValid says: Check passed!";
                return 1;
            }else
            {
                //qDebug() << "db::CheckICAOValid says: Check NOT passed! Lookup unsuccessful";
                return 0;
            }
        }else
        {
            //qDebug() << "db::CheckICAOValid says: Check NOT passed! Empty String NOT epico!";
            return 0;
        }
    }
    static QVector<double> retreiveIcaoCoordinates(QString icao)
    {
        QSqlQuery query;
        query.prepare("SELECT lat, long "
                      "FROM airports "
                      "WHERE icao = ?");
        query.addBindValue(icao);
        query.exec();

        QVector<double> result;
        while(query.next()) {
            result.append(query.value(0).toDouble());
            result.append(query.value(1).toDouble());
        }
        return result;
    }


/*
 *
 *
 * Aircraft Database Related Functions
 *
 *
 */

    static QString RetreiveRegistration(QString tail_ID)
    /* Looks up the Aircraft Registration in the Database and returns it as a string
     *
     */
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

        //qDebug() << "SQL Error: " << query.lastError().text();
        //qDebug() << "Query result: " << query.first();
        //qDebug() << query.value(2);

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
     *
     *
     * Aircraft Database Related Functions
     *
     *
     */

    static QVector<QString> retreiveSetting(QString setting_id)
    {
        QSqlQuery query;
        query.prepare("SELECT * FROM settings WHERE setting_id = ?");
        query.addBindValue(setting_id);
        query.exec();

        QVector<QString> setting;

        while(query.next()){
            setting.append(query.value(0).toString());
            setting.append(query.value(1).toString());
            setting.append(query.value(2).toString());
        }
        return setting;
    }

    static void storesetting(int setting_id, QString setting_value)
    {
        QSqlQuery query;
        query.prepare("UPDATE settings "
                      "SET  setting = ? "
                      "WHERE setting_id = ?");
        query.addBindValue(setting_value);
        query.addBindValue(setting_id);
        query.exec();
    }

};

