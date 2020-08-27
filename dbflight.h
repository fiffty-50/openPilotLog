#ifndef DBFLIGHT_H
#define DBFLIGHT_H

#include <QCoreApplication>

/*!
 * \brief The dbFlight class provides a databank interface for actions related to the
 * flights, extras and scratchpad tables, i.e. all tables that are related to a flight entry.
 */
class dbFlight
{
public:
    dbFlight();

    static QVector<QString> selectFlightById(QString flight_id);

    static bool deleteFlightById(QString flight_id);

    static QVector<QString> createFlightVectorFromInput(QString doft, QString dept, QTime tofb, QString dest,
                                                        QTime tonb, QTime tblk, QString pic, QString acft);
    static void commitFlight(QVector<QString> flight);

    static void commitToScratchpad(QVector<QString> flight);

    static QVector<QString> retreiveScratchpad();

    static bool checkScratchpad();

    static void clearScratchpad();

};

#endif // DBFLIGHT_H
