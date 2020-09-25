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
#ifndef DBFLIGHT_H
#define DBFLIGHT_H

#include <QCoreApplication>
#include "flight.h"
#include "calc.h"

/*!
 * \brief The dbFlight class provides a databank interface for actions related to the
 * flights, extras and scratchpad tables, i.e. all tables that are related to a flight entry.
 */
class dbFlight
{
public:

    static flight verifyInput(flight object);

    static flight retreiveFlight(QString flight_id);

    static bool commitFlight(flight object);



    static QVector<QString> selectFlightById(QString flight_id);

    static bool deleteFlightById(QString flight_id);

    static QVector<QString> createFlightVectorFromInput(QString doft, QString dept, QTime tofb, QString dest,
                                                        QTime tonb, QTime tblk, QString pic, QString acft);

    static void commitToScratchpad(QVector<QString> flight);

    static QVector<QString> retreiveScratchpad();

    static bool checkScratchpad();

    static void clearScratchpad();

};

#endif // DBFLIGHT_H
