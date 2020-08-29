#ifndef DBAIRPORT_H
#define DBAIRPORT_H

#include <QCoreApplication>

/*!
 * \brief The dbAirport class provides functionality for retreiving airport related
 * data from the database.
 */
class dbAirport
{
public:

    static QString retreiveAirportNameFromIcaoOrIata(QString identifier);

    static QString retreiveAirportIdFromIcao(QString identifier);

    static bool checkICAOValid(QString identifier);

    static QVector<double> retreiveIcaoCoordinates(QString icao);

    static QStringList completeIcaoOrIata(QString icaoStub);
};

#endif // DBAIRPORT_H
