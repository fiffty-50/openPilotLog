/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
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
#ifndef NIGHTTIME_H
#define NIGHTTIME_H

#include "src/calc/latlon.h"
#include "src/calc/spawrapper.h"
#include <QDateTime>
#include <QTime>
#include <QVector>

/*!
 * @class NightTime
 * @brief Provides functionality to calculate night time during a flight.
 *
 * This class calculates the portion of a flight that occurs during night conditions
 * using a one-minute resolution along the great circle route between two airports.
 * Night is defined as periods when the sun's elevation is below a configurable threshold.
 *
 * The suns elevation is determined using the SPA algorithm based on the NLR technical report
 * "Solar Position Algorithm for Solar Radiation Application" by I. Reda & A. Andreas
 */
class NightTime {
  public:
    /*!
     * \brief Encapsulates a moment in time and whether it is day or night.
     */
    struct Event {
        int timeMs;
        bool isNight;
    };

    /*!
     * \brief Calculates day/night status along a route.
     * \details For each point in the route, one minute is assumed to pass.
     * The solar elevation is evaluated at each step and compared to night_angle.
     * \param route Sequence of geographic positions along the route.
     * \param start_time_ms Start time in milliseconds since start of day.
     * \param night_angle Solar elevation threshold in degrees.
     * \return List of Event entries for each minute along the route.
     */
    static QList<Event> nightTimeForRoute(const std::vector<LatLon> &route, const QDate &date,
                                          int start_time_ms, double night_angle = -6.0);

    /*!
     * \brief Determines if an airport is in night conditions.
     * \param airport_id Row id of the airport in the database.
     * \param night_angle Solar elevation threshold in degrees.
     */
    static bool isNight(int airport_id, int date_jd, int time_ms, double night_angle = -6.0);

    /*!
     * \brief Determines if a position is in night conditions.
     * \param night_angle Solar elevation threshold in degrees.
     */
    static bool isNight(const LatLon &position, const QDate &date, const QTime &time,
                        double night_angle = -6.0);

    /*!
     * \brief Determines if a position is in night conditions.
     * \param night_angle Solar elevation threshold in degrees.
     */
    static bool isNight(const LatLon &position, int date_jd, int time_ms,
                        double night_angle = -6.0);

    /*!
     * \brief Determines if a position is in night conditions.
     * \param night_angle Solar elevation threshold in degrees.
     */
    static bool isNight(double lat, double lon, int year, int month, int day, int hour, int minute,
                        double night_angle = -6.0);

    /*!
     * \brief Calculates the solar elevation.
     * \details Uses the SPA library to compute the sun's elevation angle
     * relative to the local horizon.
     */
    static double solarElevation(const QDateTime &date_time, const LatLon &coordinate);

    /*!
     * \brief Calculates the solar elevation.
     * \details Uses the SPA library to compute the sun's elevation angle
     * relative to the local horizon.
     */
    static double solarElevation(int julian_day, int time_ms, const LatLon &coordinate);

  private:
    static constexpr double deg_to_rad(double deg) { return deg * M_PI / 180.0; }
    static constexpr double rad_to_deg(double rad) { return rad * 180.0 / M_PI; }
    static constexpr int SECS_PER_MIN  = 60;
    static constexpr int MINS_PER_HOUR = 60;
    static constexpr int SECS_PER_HOUR = SECS_PER_MIN * MINS_PER_HOUR;
    static constexpr int MS_PER_MINUTE = 60'000;
    static constexpr int MS_PER_HOUR   = MS_PER_MINUTE * 60;
    static constexpr int MS_PER_DAY    = MS_PER_HOUR * 24;

    static double elevation(double lat, double lon, int year, int month, int day, int time_ms)
    {
        SPA::data spa{};

        // Zero-initialize the structure to avoid uninitialized fields
        memset(&spa, 0, sizeof(spa));

        spa.year      = year;
        spa.month     = month;
        spa.day       = day;
        spa.hour      = time_ms / (1000 * 60 * 60);
        spa.minute    = (time_ms / (1000 * 60)) % 60;
        spa.second    = (time_ms / 1000) % 60;
        spa.latitude  = lat;
        spa.longitude = lon;
        // spa.elevation  = 0.0; - observer elevation.. maybe set to typical flight level?

        spa.pressure    = 1013;
        spa.temperature = 15;
        spa.delta_ut1   = 0;
        spa.timezone    = 0;

        SPA::calculate(&spa);
        return spa.e;
    }

    static void printSpa(const SPA::data &spa);
};

#endif // NIGHTTIME_H
