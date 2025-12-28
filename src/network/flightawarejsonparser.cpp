#include "flightawarejsonparser.h"

QList<FlightAwareFlightData> FlightAwareJsonParser::readFlights(const QByteArray &rawJsonData)
{
    // interpret the raw network reply as JSON data
    const auto replyDocument = QJsonDocument::fromJson(rawJsonData);
    DEB << replyDocument;
    const auto flightsArray = replyDocument.object()[FLIGHTS].toArray();

    // Loop over the results and parse the input
    QList<FlightAwareFlightData> flightList;
    for(const auto &flight : flightsArray) {
        const QJsonObject flightObject = flight.toObject();

        FlightAwareFlightData flightData;
        flightData.departure = flightObject[ORIGIN].toObject()[CODE_ICAO].toString();
        flightData.destination = flightObject[DESTINATION].toObject()[CODE_ICAO].toString();
        flightData.out = QDateTime::fromString(flightObject[OUT].toString(), Qt::ISODate);
        flightData.off = QDateTime::fromString(flightObject[OFF].toString(), Qt::ISODate);
        flightData.on = QDateTime::fromString(flightObject[ON].toString(), Qt::ISODate);
        flightData.in = QDateTime::fromString(flightObject[IN].toString(), Qt::ISODate);
        flightData.registration = flightObject[REGISTRATION].toString();
        flightData.iataFlightNumber = flightObject[IATA_FN].toString();
        flightData.icaoFlightNumber = flightObject[ICAO_FN].toString();
        flightList.append(flightData);
    }

    return flightList;
}
