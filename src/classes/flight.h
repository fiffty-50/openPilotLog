/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
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

    bool        isValid;
    QStringList invalidItems;             // Upon verification, verified entries are removed from the list

    int         id;                       //[1] Primary Key in Database, needed for retreival but not for commiting (sqlite autoincrement)
    QDate       doft;                     //[2] Date of Flight, initialised invalid
    QString     dept;                     //[3] Departure, initialised invalid
    QString     dest;                     //[4] Destination, initialised invalid
    QTime       tofb;                     //[5] Time off blocks (UTC), initialised invalid
    QTime       tonb;                     //[6] Time on blocks (UTC), initialised invalid
    QString     pic;                      //[7] Pilot in command (ID), initialised invalid
    QString     acft;                     //[8] Aircraft Registration (ID), initialised invalid

    QTime       tblk;                     //[9] Total Blocktime, initialised invalid
    QTime       tSPSE;                    //[10] optional times initialised as 0
    QTime       tSPME;                    //[11]
    QTime       tMP;                      //[12]
    QTime       tNIGHT;                   //[13]
    QTime       tIFR;                     //[14]

    QTime       tPIC;                     //[15]
    QTime       tPICUS;                   //[16]
    QTime       tSIC;                     //[17]
    QTime       tDUAL;                    //[18]
    QTime       tFI;                      //[19]

    QTime       tSIM;                     //[20]

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

    flight();
    flight(QVector<QString>);


    // Functions
    static flight           fromVector(QVector<QString>);
    static QVector<QString> toVector(flight);

    // Debug functionality
    void print();
    QString debug();
    operator QString() { return debug(); }
};

#endif // FLIGHT_H
