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
#ifndef ACALC_H
#define ACALC_H

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <QDateTime>
#include <QDebug>
#include "src/testing/adebug.h"
/*!
 * \brief The ACalc namespace provides various functions for calculations that are performed
 * outside of the database. This includes tasks like converting different units and formats,
 * or functions calculating block time or night time.
 */

namespace ACalc {

/*!
 * \brief ACalc::blocktime Calculates Block Time for a given departure and arrival time
 * \param tofb QTime Time Off Blocks
 * \param tonb QTime Time On Blocks
 * \return Block Time in minutes
 */
QT_DEPRECATED
inline QTime blocktime(QTime tofb, QTime tonb)
{
    QTime blocktime_out(0, 0); // initialise return value at midnight

    if (tonb > tofb) { // landing same day
        int blockseconds = tofb.secsTo(tonb);
        blocktime_out = blocktime_out.addSecs(blockseconds);
    } else { // landing next day
        QTime midnight(0, 0);
        int blockseconds = tofb.secsTo(midnight);
        blocktime_out = blocktime_out.addSecs(blockseconds);
        blockseconds = midnight.secsTo(tonb);
        blocktime_out = blocktime_out.addSecs(blockseconds);
    }
    return blocktime_out;
}
/*!
 * \brief ACalc::minutes_to_string Converts database time to String Time
 * \param blockminutes from database
 * \return String hh:mm
 */
QT_DEPRECATED
inline QString minutesToString(QString block_minutes)
{
    int minutes = block_minutes.toInt();
    QString hour = QString::number(minutes / 60);
    if (hour.size() < 2) {
        hour.prepend("0");
    }
    QString minute = QString::number(minutes % 60);
    if (minute.size() < 2) {
        minute.prepend("0");
    }
    QString block_time = hour + ":" + minute;
    return block_time;
};

QT_DEPRECATED
inline QString minutesToString(int block_minutes)
{
    QString hour = QString::number(block_minutes / 60);
    if (hour.size() < 2) {
        hour.prepend("0");
    }
    QString minute = QString::number(block_minutes % 60);
    if (minute.size() < 2) {
        minute.prepend("0");
    }
    QString blocktime = hour + ":" + minute;
    return blocktime;
};

/*!
 * \brief ACalc::time_to_minutes converts QTime to int minutes
 * \param time QTime
 * \return int time as number of minutes
 */
QT_DEPRECATED
inline int QTimeToMinutes(QTime time)
{
    QString timestring = time.toString("hh:mm");
    int minutes = (timestring.left(2).toInt()) * 60;
    minutes += timestring.right(2).toInt();
    return minutes;
}

/*!
 * \brief ACalc::string_to_minutes Converts String Time to String Number of Minutes
 * \param timestring "hh:mm"
 * \return String number of minutes
 */
QT_DEPRECATED
inline int stringToMinutes(QString timestring)
{
    int minutes = (timestring.left(2).toInt()) * 60;
    minutes += timestring.right(2).toInt();
    timestring = QString::number(minutes);
    return minutes;
}

/*!
 * \brief radToDeg Converts radians to degrees
 * \param rad
 * \return degrees
 */
inline double radToDeg(double rad)
{
    double deg = rad * (180 / M_PI);
    return deg;
}

/*!
 * \brief degToRad Converts degrees to radians
 * \param deg
 * \return radians
 */
inline double degToRad(double deg)
{
    double rad = deg * (M_PI / 180);
    return rad;
}

/*!
 * \brief radToNauticalMiles Convert Radians to nautical miles
 * \param rad
 * \return nautical miles
 */
inline double radToNauticalMiles(double rad)
{
    double nm = rad * 3440.06479482;
    return nm;
}

/*!
 * \brief greatCircleDistance Calculates Great Circle distance between two coordinates, return in Radians.
 * \param lat1 Location Latitude in degrees -90:90 ;S(-) N(+)
 * \param lon1 Location Longitude in degrees -180:180 W(-) E(+)
 * \param lat2 Location Latitude in degrees -90:90 ;S(-) N(+)
 * \param lon2 Location Longitude in degrees -180:180 W(-) E(+)
 * \return
 */
double greatCircleDistance(double lat1, double lon1, double lat2, double lon2);

/*!
 * \brief ACalc::greatCircleDistanceBetweenAirports Calculates Great
 * Circle distance between two coordinates, return in nautical miles.
 * \param dept ICAO 4-letter Airport Identifier
 * \param dest ICAO 4-letter Airport Identifier
 * \return Nautical Miles From Departure to Destination
 */
double greatCircleDistanceBetweenAirports(const QString &dept, const QString &dest);

/*!
 * \brief  Calculates a list of points (lat,lon) along the Great Circle between two points.
 * The points are spaced equally, one minute of block time apart.
 * \param lat1 Location Latitude in degrees -90:90 ;S(-) N(+)
 * \param lon1 Location Longitude in degrees -180:180 W(-) E(+)
 * \param lat2 Location Latitude in degrees -90:90 ;S(-) N(+)
 * \param lon2 Location Longitude in degrees -180:180 W(-) E(+)
 * \param tblk Total Blocktime in minutes
 * \return coordinates {lat,lon} along the Great Circle Track
 */
QVector<QVector<double>> intermediatePointsOnGreatCircle(double lat1,
                                                         double lon1,
                                                         double lat2,
                                                         double lon2,
                                                         int tblk);
/*!
 * \brief Calculates solar elevation angle for a given point in time and latitude/longitude coordinates
 *
 * It is based on the formulas found here: http://stjarnhimlen.se/comp/tutorial.html#5
 *
 * Credit also goes to Darin C. Koblick for his matlab implementation of various of these
 * formulas and to Kevin Godden for porting it to C++.
 *
 * Darin C. Koblock: https://www.mathworks.com/matlabcentral/profile/authors/1284781
 * Kevin Godden: https://www.ridgesolutions.ie/index.php/about-us/
 *
 * \param utc_time_point - QDateTime (UTC) for which the elevation is Calculated
 * \param lat - Location Latitude in degrees -90:90 ;S(-) N(+)
 * \param lon - Location Longitude in degrees -180:180 W(-) E(+)
 * \return elevation - double of solar elevation in degrees.
 */
double solarElevation(QDateTime utc_time_point, double lat, double lon);

/*!
 * \brief Calculates which portion of a flight was conducted in night conditions.
 * \param dept - ICAO 4-letter code of Departure Airport
 * \param dest - ICAO 4-letter Code of Destination Airport
 * \param departureTime - QDateTime of Departure (UTC)
 * \param tblk - Total block time in minutes
 * \param nightAngle - the solar elevation angle where night conditons exist.
 * Default -6 (end of civil evening twilight)
 * \return Total number of minutes under night flying conditions
 */
int calculateNightTime(const QString &dept, const QString &dest, QDateTime departureTime, int tblk, int nightAngle);

bool isNight(const QString &icao, QDateTime event_time, int night_angle);

QString formatTimeInput(QString user_input);

void updateAutoTimes(int acft_id);

void updateNightTimes();
} // namespace ACalc

#endif // ACALC_H
