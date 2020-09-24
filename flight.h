#ifndef FLIGHT_H
#define FLIGHT_H

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>

/*!
 * \brief The flight class is a container class for a logbook entry. It contains all the
 * entries relevant to a flight. There are 8 mandatory entries which are initalized
 * invalid and need to be set before the flight can be committed. The other entries
 * are optional and can be empty, null or any other value.
 * All time entries in a flight object shall be UTC.
 */
class flight
{
public:
    bool        isValid = false;
    QStringList invalidItems = {                      // Upon verification, verified entries are removed from the list
        "doft", "dept", "dest", "tofb",
        "tonb", "pic",  "acft", "tblk"
    };

    int         id      = -1;                         //[1] Primary Key in Database, needed for retreival but not for commiting (sqlite autoincrement)
    QDate       doft    = QDate();                    //[2] Date of Flight, initialised invalid
    QString     dept    = "INVA";                     //[3] Departure, initialised invalid
    QString     dest    = "INVA";                     //[4] Destination, initialised invalid
    QTime       tofb    = QTime();                    //[5] Time off blocks (UTC), initialised invalid
    QTime       tonb    = QTime();                    //[6] Time on blocks (UTC), initialised invalid
    QString     pic     = "INVA";                     //[7] Pilot in command (ID), initialised invalid
    QString     acft    = "INVA";                     //[8] Aircraft Registration (ID), initialised invalid

    QTime       tblk    = QTime();                    //[9] Total Blocktime, initialised invalid
    QTime       tSPSE   = QTime(0,0);                 //[10] optional times initialised as 0
    QTime       tSPME   = QTime(0,0);                 //[11]
    QTime       tMP     = QTime(0,0);                 //[12]
    QTime       tNIGHT  = QTime(0,0);                 //[13]
    QTime       tIFR    = QTime(0,0);                 //[14]

    QTime       tPIC    = QTime(0,0);                 //[15]
    QTime       tPICUS  = QTime(0,0);                 //[16]
    QTime       tSIC    = QTime(0,0);                 //[17]
    QTime       tDUAL   = QTime(0,0);                 //[18]
    QTime       tFI     = QTime(0,0);                 //[19]

    QTime       tSIM    = QTime(0,0);                 //[20]

    int         pilotFlying;                          //[21]
    int         toDay;                                //[22]
    int         toNight;                              //[23]
    int         ldgDay;                               //[24]
    int         ldgNight;                             //[25]
    int         autoland;                             //[26]

    QString     secondPilot;                          //[27]
    QString     thirdPilot;                           //[28]
    QString     approachType;                         //[29]
    QString     flightNumber;                         //[30]
    QString     remarks;                              //[31]

    // Functions
    static flight           fromVector(QVector<QString>);
    static QVector<QString> toVector(flight);

    // Debug functionality
    void printFlight();
    QString debug();
    operator QString() { return debug(); }
};

#endif // FLIGHT_H
