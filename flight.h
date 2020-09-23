#ifndef FLIGHT_H
#define FLIGHT_H

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>

/*!
 * \brief The flight class is a container class for a logbook entry. It contains all the
 * entries relevant to a flight. There are 8 mandatory entries which are initalized
 * invalid and need to be set before the flight can be committed. The other entries
 * are optional. All time entries in a flight object shall be UTC.
 */
class flight
{
public:
    bool        isValid = false;
    QStringList invalidItems = {
        "doft", "dept", "dest", "tofb",
        "tonb", "pic",  "acft", "tblk"
    };

    int         id      = -1;                             // Primary Key in Database, needed for retreival but not for commiting (sqlite autoincrement)
    QDate       doft    = QDate();                        // Date of Flight
    QString     dept    = "INVA";                         // Departure
    QString     dest    = "INVA";                         // Destination
    QTime       tofb    = QTime();                        // Time off blocks (UTC), initialised invalid
    QTime       tonb    = QTime();                        // Time on blocks (UTC), initialised invalid
    QString     pic     = "INVA";                         // Pilot in command (ID)
    QString     acft    = "INVA";                         // Aircraft Registration (ID)

    QTime       tblk    = QTime();                        // Total Blocktime, initialised invalid
    QTime       tSPSE   = QTime(0,0);                     // optional times initialised as 0
    QTime       tSPME   = QTime(0,0);
    QTime       tMP     = QTime(0,0);
    QTime       tNIGHT  = QTime(0,0);
    QTime       tIFR    = QTime(0,0);

    QTime       tPIC    = QTime(0,0);
    QTime       tSIC    = QTime(0,0);
    QTime       tDUAL   = QTime(0,0);
    QTime       tFI     = QTime(0,0);

    QTime       tSIM    = QTime(0,0);

    int         pilotFlying;
    int         toDay;
    int         toNight;
    int         ldgDay;
    int         ldgNight;
    int         autoland;

    QString     secondPilot;
    QString     thirdPilot;
    QString     approachType;
    QString     flightNumber;
    QString     remarks;

    // Functions
    static flight           fromVector(QVector<QString>);
    static QVector<QString> toVector(flight);

    // Debug functionality
    void printFlight();
    QString debug();
    operator QString() { return debug(); }
};

#endif // FLIGHT_H
