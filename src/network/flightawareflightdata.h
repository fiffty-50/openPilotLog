#ifndef FLIGHTAWAREFLIGHTDATA_H
#define FLIGHTAWAREFLIGHTDATA_H

#include <QDateTime>
#include "src/opl.h"

/*!
 * \brief A struct for storing data retreived from the FlightAware API
 */
class FlightAwareFlightData
{
public:
    FlightAwareFlightData() = default;

    QString departure;
    QString destination;
    QString registration;
    QString icaoFlightNumber;
    QString iataFlightNumber;
    QDateTime out, off, on, in;

    void print() const {
        DEB << "Flight Aware Data:";
        DEB << "Departure: " << departure << "Destination" << destination;
        DEB << "Out:" << out << "Off:" << off << "On:" << on << "in:" << in;
        DEB << "Icao #" << icaoFlightNumber << "Iata #" << iataFlightNumber << "Registration" << registration;
    }
};

#endif // FLIGHTAWAREFLIGHTDATA_H
