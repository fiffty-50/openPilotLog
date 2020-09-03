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
#include "dbairport.h"
#include "dbman.cpp"

/*!
     * \brief RetreiveAirportNameFromIcaoOrIata Looks up Airport Name
     * \param identifier can be ICAO or IATA airport codes.
     * \return The name of the airport associated with the above code
     */
QString dbAirport::retreiveAirportNameFromIcaoOrIata(QString identifier)
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

QString dbAirport::retreiveAirportIdFromIcao(QString identifier)
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

QStringList dbAirport::completeIcaoOrIata(QString icaoStub)
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

/*!
 * \brief dbAirport::retreiveIataIcaoList Provides a QStringList of airport codes
 * in the database
 * \return ICAO and IATA codes in the database
 */
QStringList dbAirport::retreiveIataIcaoList()
{
    QSqlQuery query;
    query.prepare("SELECT icao, iata from airports");
    query.exec();

    QStringList result;
    while(query.next())
    {
        result.append(query.value(0).toString());
        result.append(query.value(1).toString());
    }
    result.removeAll(QString(""));
    return result;
}
/*!
     * \brief CheckICAOValid Verifies if a user input airport exists in the database
     * \param identifier can be ICAO or IATA airport codes.
     * \return bool if airport is in database.
     */
bool dbAirport::checkICAOValid(QString identifier)
{
    if(identifier.length() == 4)
    {
        QString check = retreiveAirportIdFromIcao(identifier);
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

/*!
     * \brief retreiveIcaoCoordinates Looks up coordinates (lat,long) for a given airport
     * \param icao 4-letter code for the airport
     * \return {lat,lon} in decimal degrees
     */
QVector<double> dbAirport::retreiveIcaoCoordinates(QString icao)
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
