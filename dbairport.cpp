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