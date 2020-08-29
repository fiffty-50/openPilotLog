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

#ifndef CALC_H
#define CALC_H

#include "dbairport.h"
#include <QDateTime>
#include <cmath>
#include <QDebug>
/*!
 * \brief The calc class provides functionality for various calculations that are performed
 * outside of the database. This includes tasks like converting different units and formats,
 * or functions calculating block time or night time.
 */
class calc
{
public:

    static QTime blocktime(QTime tofb, QTime tonb);

    static QString minutes_to_string(QString blockminutes);

    static int string_to_minutes(QString time);

    static int time_to_minutes(QTime time);

    static double radToDeg(double rad);

    static double degToRad(double deg);

    static double radToNauticalMiles(double rad);

    static double greatCircleDistance(double lat1, double lon1, double lat2, double lon2);

    static QVector<QVector<double>> intermediatePointsOnGreatCircle(double lat1, double lon1, double lat2, double lon2, int tblk);

    static double solarElevation(QDateTime utc_time_point, double lat, double lon);

    static int calculateNightTime(QString dept, QString dest, QDateTime departureTime, int tblk);
};


#endif // CALC_H
