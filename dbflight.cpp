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
#include "dbpilots.h"
#include "dbaircraft.h"


/*!
 * \brief dbFlight::verifyInput Checks the validity of the data in a
 * flight object against the database and sets its verified flag
 * accordingly. Information about partial validity can be obtained
 * by querying the flight objects invalidItems member.
 */
void dbFlight::verifyInput(flight object)
{

    if (object.doft.isValid()){
        object.invalidItems.removeOne("doft");
        qDebug() << "doft verified.";
    }else{
        qWarning() << "Invalid entry: doft";
    }

    if(dbAirport::checkICAOValid(object.dept)){
        object.invalidItems.removeOne("dept");
        qDebug() << "dept verified.";
    }else{
        qWarning() << "Invalid entry: dept";
    }

    if(dbAirport::checkICAOValid(object.dest)){
        object.invalidItems.removeOne("dest");
        qDebug() << "dest verified.";
    }else{
        qWarning() << "Invalid entry: dest";
    }

    if(object.tofb.isValid()){
        object.invalidItems.removeOne("tofb");
        qDebug() << "tofb verified.";
    }else{
        qWarning() << "Invalid entry: tofb";
    }

    if(object.tonb.isValid()){
        object.invalidItems.removeOne("tonb");
        qDebug() << "tonb verified.";
    }else{
        qWarning() << "Invalid entry: tonb";
    }

    if(dbPilots::verifyPilotExists(object.pic.split(QLatin1Char(',')))){
        object.invalidItems.removeOne("pic");
        qDebug() << "pic verified.";
    }else{
        qWarning() << "Invalid entry: pic";
    }

    //acft

    QTime tblkcheck = calc::blocktime(object.tofb, object.tonb);
    if(object.tblk.isValid() && !object.tblk.isNull() && tblkcheck == object.tblk){
        object.invalidItems.removeOne("tblk");
        qDebug() << "tblk verified.";
    }else{
        qWarning() << "Invalid entry: tblk";
    }

    if(!dbAircraft::retreiveTailId(object.acft).isEmpty()){
        object.invalidItems.removeOne("acft");
        qDebug() << "acft verified.";
    }else{
        qWarning() << "Invalid entry: acft";
    }

    if(object.invalidItems.isEmpty()){
        object.isValid = true;
        qDebug() << "All checks passed. Object is now verified.";
    }else{
        qDebug() << "Not all checks have been passed.";
        qDebug() << "Invalid items: " << object.invalidItems;
    }

    /* To Do: Checks on internal logic, i.e. times <= tblk,
     * restrict what strings are allowed in remarks etc.

    object.pic     = details[7];
    object.acft    = details[8];
    object.tblk    = QTime::fromString(details[9],"hh:mm");*/

    /*object.tSPSE   = QTime::fromString(details[10],"hh:mm");
    object.tSPME   = QTime::fromString(details[11],"hh:mm");
    object.tMP     = QTime::fromString(details[12],"hh:mm");
    object.tNIGHT  = QTime::fromString(details[13],"hh:mm");
    object.tIFR    = QTime::fromString(details[14],"hh:mm");

    object.tPIC    = QTime::fromString(details[15],"hh:mm");
    object.tPICUS  = QTime::fromString(details[16],"hh:mm");
    object.tSIC    = QTime::fromString(details[17],"hh:mm");
    object.tDUAL   = QTime::fromString(details[18],"hh:mm");
    object.tFI     = QTime::fromString(details[19],"hh:mm");
    object.tSIM    = QTime::fromString(details[20],"hh:mm");

    object.pilotFlying  = details[21].toInt();
    object.toDay        = details[22].toInt();
    object.toNight      = details[23].toInt();
    object.ldgDay       = details[24].toInt();
    object.ldgNight     = details[25].toInt();
    object.autoland     = details[26].toInt();

    object.secondPilot  = details[27];
    object.thirdPilot   = details[28];
    object.approachType = details[29];
    object.flightNumber = details[30];
    object.remarks      = details[31];*/
}

/*!
 * \brief dbFlight::retreiveFlight Runs a select statement on the database
 * for a given flight_id and returns a flight object containing the details
 * for the given ID. If no matching ID is found, an empty flight object is
 * returned
 * \param flight_id Primary Key of flight database
 */
flight dbFlight::retreiveFlight(QString flight_id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM flights WHERE id = ?");
    query.addBindValue(flight_id);
    query.exec();

    if(query.first());
    else
    {
        qWarning() << __func__ << "No Flight with this ID found";
        return flight(); //return empty

    }
    flight object;

    object.id      = query.value(0).toInt();
    object.doft    = QDate::fromString(query.value(1).toString(),Qt::ISODate);
    object.dept    = query.value(2).toString();
    object.dest    = query.value(3).toString();
    object.tofb    = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(4).toString()),"hh:mm");
    object.tonb    = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(5).toString()),"hh:mm");
    object.pic     = dbPilots::retreivePilotNameFromID(
                     query.value(6).toString());
    object.acft    = dbAircraft::retreiveRegistration(
                     query.value(7).toString());
    object.tblk    = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(8).toString()),"hh:mm");
    object.tSPSE   = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(9).toString()),"hh:mm");
    object.tSPME   = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(10).toString()),"hh:mm");
    object.tMP     = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(11).toString()),"hh:mm");
    object.tNIGHT  = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(12).toString()),"hh:mm");
    object.tIFR    = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(13).toString()),"hh:mm");

    object.tPIC    = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(14).toString()),"hh:mm");
    object.tPICUS  = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(15).toString()),"hh:mm");
    object.tSIC    = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(16).toString()),"hh:mm");
    object.tDUAL   = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(17).toString()),"hh:mm");
    object.tFI     = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(18).toString()),"hh:mm");
    object.tSIM    = QTime::fromString(
                     calc::minutes_to_string(
                     query.value(19).toString()),"hh:mm");

    object.pilotFlying  = query.value(20).toInt();
    object.toDay        = query.value(21).toInt();
    object.toNight      = query.value(22).toInt();
    object.ldgDay       = query.value(23).toInt();
    object.ldgNight     = query.value(24).toInt();
    object.autoland     = query.value(25).toInt();

    object.secondPilot  = query.value(26).toInt();
    object.thirdPilot   = query.value(27).toInt();
    object.approachType = query.value(28).toString();
    object.flightNumber = query.value(29).toString();
    object.remarks      = query.value(30).toString();

    //Database entries are assumed to be valid
    object.isValid      = true;
    object.invalidItems.clear();

    return object;
}


bool dbFlight::commitFlight(flight object)
{
    //To Do
    qDebug() << object;
    return false;
}

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
