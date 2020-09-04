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
#include "dbflight.h"
#include "calc.h"

/*!
 * \brief SelectFlightById Retreives a single flight from the database.
 * \param flight_id Primary Key of flights database
 * \return Flight details of selected flight.
 */
QVector<QString> dbFlight::selectFlightById(QString flight_id)
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

    qDebug() << "db::SelectFlightById - retreived flight: " << flight;
    return flight;
}

/*!
 * \brief deleteFlightById Deletes a Flight from the database.
 * Entries in the basic flights table as well as in the extras table are deleted.
 * \param flight_id The primary key of the entry in the database
 * \return True if no errors, otherwise false
 */
bool dbFlight::deleteFlightById(QString flight_id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM flights WHERE id = ?");
    query.addBindValue(flight_id);
    query.exec();
    QString error = query.lastError().text();

    QSqlQuery query2;
    query2.prepare("DELETE FROM extras WHERE extras_id = ?");
    query2.addBindValue(flight_id);
    query2.exec();
    QString error2 = query2.lastError().text();

    qDebug() << "db::deleteFlightById: Removing flight with ID#: " << flight_id;
    if(error.length() > 0 || error2.length() > 0)
    {
        qWarning() << "db::deleteFlightsById: Errors have occured: " << error << " " << error2;
        return false;
    }else
    {
        return true;
    }
}

/*!
 * \brief CreateFlightVectorFromInput Converts input from NewFlight Window into database format
 * \param doft Date of flight
 * \param dept Place of Departure
 * \param tofb Time Off Blocks (UTC)
 * \param dest Place of Destination
 * \param tonb Time On Blocks (UTC)
 * \param tblk Total Block Time
 * \param pic Pilot in command
 * \param acft Aircraft
 * \return Vector of values ready for committing
 */
QVector<QString> dbFlight::createFlightVectorFromInput(QString doft, QString dept, QTime tofb, QString dest,
                                             QTime tonb, QTime tblk, QString pic, QString acft)
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
/*!
 * \brief CommitFlight Inserts prepared flight vector into database. Also creates
 * a corresponding entry in the extras database to ensure matching IDs.
 * \param flight a Vector of values in database format
 */
void dbFlight::commitFlight(QVector<QString> flight)// flight vector shall always have length 9
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
}
/*!
 * \brief CommitToScratchpad Commits the inputs of the NewFlight window to a scratchpad
 * to make them available for restoring entries when the input fields are being reloaded.
 * \param flight The input data, which was not accepted for commiting to the flights table.
 */
void dbFlight::commitToScratchpad(QVector<QString> flight)// to store input mask
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
}

/*!
 * \brief RetreiveScratchpad Selects data from scratchpad
 * \return Vector of data contained in scratchpad
 */
QVector<QString> dbFlight::retreiveScratchpad()
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

    query.previous();
    QVector<QString> flight;
    while (query.next()) {
        flight.append(query.value(0).toString());
        flight.append(query.value(1).toString());
        flight.append(query.value(2).toString());
        flight.append(calc::minutes_to_string((query.value(3).toString())));
        flight.append(query.value(4).toString());
        flight.append(calc::minutes_to_string((query.value(5).toString())));
        flight.append(calc::minutes_to_string((query.value(6).toString())));
        flight.append(query.value(7).toString());
        flight.append(query.value(8).toString());
    }
    clearScratchpad();
    return flight;
}

/*!
 * \brief CheckScratchpad Verifies if the scratchpad contains data
 * \return true if scratchpad contains data
 */
bool dbFlight::checkScratchpad() // see if scratchpad is empty
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
/*!
 * \brief ClearScratchpad Deletes data contained in the scratchpad
 */
void dbFlight::clearScratchpad()
{
    qDebug() << "Deleting scratchpad";
    QSqlQuery query;
    query.prepare("DELETE FROM scratchpad;");
    query.exec();
}
