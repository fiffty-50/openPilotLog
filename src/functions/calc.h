/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <QDateTime>
#include <QDebug>
#include "src/classes/time.h"
/*!
 * \brief The ACalc namespace provides various functions for calculations that are performed
 * outside of the database. This includes tasks like converting different units and formats,
 * or functions calculating block time or night time.
 */

namespace OPL::Calc {

/*!
 * \brief radToDeg Converts radians to degrees
 * \param rad
 * \return degrees
 */
inline double radToDeg(double rad)
{
    return rad * (180 / M_PI);
}

/*!
 * \brief degToRad Converts degrees to radians
 * \param deg
 * \return radians
 */
inline double degToRad(double deg)
{
    return deg * (M_PI / 180);
}

/*!
 * \brief radToNauticalMiles Convert Radians to nautical miles
 * \param rad
 * \return nautical miles
 */
inline double radToNauticalMiles(double rad)
{
    return rad * 3440.06479482;
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
 * \brief Opl::Calc::greatCircleDistanceBetweenAirports Calculates Great
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
double solarElevation(const QDateTime& utc_time_point, double lat, double lon);

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
int calculateNightTime(const QString &dept, const QString &dest, const QDateTime& departureTime, int tblk, int nightAngle);

bool isNight(const QString &icao, const QDateTime &event_time, int night_angle);

QString formatTimeInput(QString user_input);

void updateAutoTimes(int acft_id);

void updateNightTimes();

/*!
 * \brief The NightTimeValues struct encapsulates values relating to night time that are needed by the NewFlightDialog
 */
struct NightTimeValues{
    NightTimeValues() = delete;
    NightTimeValues(const QString& dept, const QString& dest, const QDateTime& departure_time, int block_minutes, int night_angle)
    {
        nightMinutes = calculateNightTime(dept, dest, departure_time, block_minutes, night_angle);

        OPL::Time nightTime = OPL::Time(nightMinutes, DateTimeFormat());
        OPL::Time totalTime = OPL::Time(block_minutes, DateTimeFormat());

        if (nightMinutes == 0) { // all day
            takeOffNight = false;
            landingNight  = false;
        }
        else if (nightMinutes == block_minutes) { // all night
            takeOffNight = true;
            landingNight  = true;
        } else {
            if(isNight(dept, departure_time,  night_angle))
                takeOffNight = true;
            else
                takeOffNight = false;
            if(isNight(dest, departure_time.addSecs(block_minutes * 60), night_angle))
                landingNight = true;
            else
                landingNight = false;
        }

    };

//    NightTimeValues(bool to_night, bool ldg_night, int night_minutes, OPL::Time night_time, OPL::Time total_time)
//        : takeOffNight(to_night), landingNight(ldg_night), nightMinutes(night_minutes), nightTime(night_time), totalTime(total_time){};
    bool takeOffNight;
    bool landingNight;
    int nightMinutes;
//    OPL::Time nightTime;
//    OPL::Time totalTime;

    inline bool isAllDay()      {return (!takeOffNight  && !landingNight);}
    inline bool isAllNight()    {return ( takeOffNight  &&  landingNight);}
    inline bool isDayToNight()  {return (!takeOffNight  &&  landingNight);}
    inline bool isNightToDay()  {return ( takeOffNight  && !landingNight);}
};


} // namespace OPL::Calc

#endif // CALC_H
