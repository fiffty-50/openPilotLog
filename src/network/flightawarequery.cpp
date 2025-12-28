#include "flightawarequery.h"
#include "flightawarejsonparser.h"
#include <QNetworkReply>

FlightAwareQuery::FlightAwareQuery(QObject *parent)
    : QObject{parent}
{
    m_networkManager = new QNetworkAccessManager(this);
}

QList<FlightAwareFlightData> FlightAwareQuery::getFlightData(const QString &flightId, const QDate &date)
{
    LOG << "Preparing query data";
    // prepare the query data
    const QDateTime startDateTime = QDateTime(date, QTime(0, 0));
    const QDateTime endDateTime = QDateTime(date, QTime(23, 59));
    const QString queryUrl = QUERY_BASE_URL + flightId + QString("?start=%1&end=%2").arg(startDateTime.toString(Qt::ISODate), endDateTime.toString(Qt::ISODate));

    // construct the network request
    const QByteArray apiKey = Settings::getFlightAwareApiKey();
    if (apiKey.isEmpty()) {
        LOG << "API key not found.";
        return {};
    }
    LOG << "Sending network request: " << queryUrl << "with API key: " << apiKey;
    QNetworkRequest request;
    request.setRawHeader("x-apikey", apiKey);
    request.setRawHeader("Accept", "application/json; charset=UTF-8");
    request.setUrl(queryUrl);

    // invoke the network manager to execute the request
    m_networkReply = m_networkManager->get(request);
    // QObject::connect(m_networkReply, &QNetworkReply::finished,
    //                  this,           &FlightAwareQuery::replyFinished);


    // Wait for finishing the network request or a timeout
    LOG << "Setting timer and staring event loop";
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, this, &FlightAwareQuery::timeOut);
    timer.start(5000); //5s

    QEventLoop loop;
    QObject::connect(m_networkReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    LOG << "Reading reply...";
    // check the reply
    if(m_networkReply->error() != QNetworkReply::NoError) {
        LOG << m_networkReply->error();
        LOG << m_networkReply->errorString();
        return {};
    }

    LOG << "Parsing reply...";
    // parse the reply and return the result
    return FlightAwareJsonParser::readFlights(m_networkReply->readAll());
}

// void FlightAwareQuery::replyFinished()
// {
//     if(m_networkReply->error() != QNetworkReply::NoError) {
//         qDebug() << m_networkReply->error();
//         qDebug() << m_networkReply->errorString();
//         return;
//     }

//     // parse the reply
//     m_flightList.clear();
//     m_flightList = FlightAwareJsonParser::readFlights(m_networkReply->readAll());
//     emit readyToRead();
// }

void FlightAwareQuery::timeOut()
{
    m_networkReply->abort(); // triggers replyFinished() with an error code
}

