#ifndef FLIGHTAWAREQUERY_H
#define FLIGHTAWAREQUERY_H
#include <QNetworkAccessManager>
#include "src/network/flightawareflightdata.h"
#include "src/classes/settings.h"

/*!
 * \brief The FlightAwareQuery class handles API requests to FlightAware
 * \details The FlightAware API allows retreiving most of the details required to log a flight.
 * Given a date and a flight number, which can either be an icao or iata flight id, most
 * base data can be auto-populated with only little user input required to complete a given flight.
 *
 * When the Flight Data is ready to be read (i.e. network request finished and data parsed), readyToRead()
 * is emitted and the parsed data can be retreived by calling flightList(). A list is used because some flights
 * are multi-sector flights that share a common flight number.
 */
class FlightAwareQuery : public QObject
{
    Q_OBJECT
    QNetworkAccessManager *m_networkManager;
    QNetworkReply* m_networkReply;

    const static inline QString QUERY_BASE_URL = QStringLiteral("https://aeroapi.flightaware.com/aeroapi/flights/");
    const QByteArray getApiKey() const { return Settings::getFlightAwareApiKey(); }

public:
    explicit FlightAwareQuery(QObject *parent = nullptr);
    QList<FlightAwareFlightData> getFlightData(const QString &flightId, const QDate &date);

// signals:
//     void readyToRead();

private slots:
    // void replyFinished();
    void timeOut();

};

#endif // FLIGHTAWAREQUERY_H
