#ifndef FLIGHTAWAREJSONPARSER_H
#define FLIGHTAWAREJSONPARSER_H

#include "flightawareflightdata.h"

/*!
 * \brief The FlightAwareJsonParser reads the reply of a call to the FlightAware API and extracts
 * the relevant data points
 */
class FlightAwareJsonParser
{
public:
    FlightAwareJsonParser() = delete;

    /*!
     * \brief read the reply of a FlightAware API call and parse the data into a list of FlightAwareData objects
     */
    static QList<FlightAwareFlightData> readFlights(const QByteArray &rawJsonData);
private:
    const static inline QString FLIGHTS = QStringLiteral("flights");
    const static inline QString ORIGIN = QStringLiteral("origin");
    const static inline QString CODE_ICAO = QStringLiteral("code_icao");
    const static inline QString CODE_IATA = QStringLiteral("code_iata");
    const static inline QString DESTINATION = QStringLiteral("destination");
    const static inline QString OUT = QStringLiteral("actual_out");
    const static inline QString OFF = QStringLiteral("actual_off");
    const static inline QString ON = QStringLiteral("actual_on");
    const static inline QString IN = QStringLiteral("actual_in");
    const static inline QString REGISTRATION = QStringLiteral("registration");
    const static inline QString IATA_FN = QStringLiteral("ident_iata");
    const static inline QString ICAO_FN = QStringLiteral("ident_icao");
};

#endif // FLIGHTAWAREJSONPARSER_H
