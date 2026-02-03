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
#include "nighttime.h"
#include "src/calc/spa.h"
#include "src/classes/date.h"
#include "src/classes/time.h"
#include "src/database/airportgeographicalinfo.h"
#include <qdatetime.h>

double NightTime::solarElevation(int julian_day, int time_ms, const LatLon &coordinate)
{
    assert(OPL::Date::julianDayIsValid(julian_day));
    assert(OPL::Time::isValidBlockTime(time_ms));
    spa_data spa;
    memset(&spa, 0, sizeof(spa));

    const QDate date = QDate::fromJulianDay(julian_day);
    spa.year         = date.year();
    spa.month        = date.month();
    spa.day          = date.day();

    const QTime time = QTime::fromMSecsSinceStartOfDay(time_ms);
    spa.hour         = time.hour();
    spa.minute       = time.minute();
    spa.latitude     = coordinate.lat;
    spa.longitude    = coordinate.lon;

    printSpa(spa);
    spa_calculate(&spa);
    return spa.e;
}

void NightTime::printSpa(const spa_data &spa)
{
    DEB << "Spa: " << "Date Time: " << spa.year << '/' << spa.month << '/' << spa.day << " - "
        << spa.hour << ':' << spa.minute;
    DEB << "Pos: " << '[' << spa.latitude << ',' << spa.longitude;
}

double NightTime::solarElevation(const QDateTime &date_time, const LatLon &coordinate)
{
    assert(date_time.isValid());
    spa_data spa;
    memset(&spa, 0, sizeof(spa));

    spa.year      = date_time.date().year();
    spa.month     = date_time.date().month();
    spa.day       = date_time.date().day();
    spa.hour      = date_time.time().hour();
    spa.minute    = date_time.time().minute();
    spa.latitude  = coordinate.lat;
    spa.longitude = coordinate.lon;

    spa_calculate(&spa);
    return spa.e;
}

bool NightTime::isNight(int airport_id, int date_jd, int time_ms, double night_angle)
{
    const LatLon position = airportGeoData->coordinates(airport_id);
    return isNight(position, date_jd, time_ms, night_angle);
}

bool NightTime::isNight(const LatLon &position, const QDate &date, const QTime &time,
                        double night_angle)
{
    assert(date.isValid());
    assert(time.isValid());
    spa_data spa;
    memset(&spa, 0, sizeof(spa));

    spa.year  = date.year();
    spa.month = date.month();
    spa.day   = date.day();

    spa.hour      = time.hour();
    spa.minute    = time.minute();
    spa.latitude  = position.lat;
    spa.longitude = position.lon;

    spa_calculate(&spa);

    return spa.e < night_angle;
}

bool NightTime::isNight(const LatLon &position, int date_jd, int time_ms, double night_angle)
{
    assert(OPL::Date::julianDayIsValid(date_jd));
    assert(OPL::Time::isValidBlockTime(time_ms));
    spa_data spa;
    memset(&spa, 0, sizeof(spa));

    const QDate date = QDate::fromJulianDay(date_jd);
    spa.year         = date.year();
    spa.month        = date.month();
    spa.day          = date.day();

    const QTime time = QTime::fromMSecsSinceStartOfDay(time_ms);
    spa.hour         = time.hour();
    spa.minute       = time.minute();
    spa.latitude     = position.lat;
    spa.longitude    = position.lon;

    spa_calculate(&spa);

    return spa.e < night_angle;
}

bool NightTime::isNight(double lat, double lon, int year, int month, int day, int hour, int minute,
                        double night_angle)
{
    spa_data spa;
    memset(&spa, 0, sizeof(spa));

    spa.year  = year;
    spa.month = month;
    spa.day   = day;

    spa.hour      = hour;
    spa.minute    = minute;
    spa.latitude  = lat;
    spa.longitude = lon;

    spa_calculate(&spa);

    return spa.e < night_angle;
}

QList<NightTime::Event> NightTime::nightTimeForRoute(const std::vector<LatLon> &route,
                                                     const QDate &date, int time_ms,
                                                     double night_angle)
{
    // We ignore midnight crossings and assume the same date for all calculations as the changes in
    // sun elevation from one day to another are negligigble

    spa_data spa;
    memset(&spa, 0, sizeof(spa));
    // Set constants
    spa.year        = date.year();
    spa.month       = date.month();
    spa.day         = date.day();
    spa.pressure    = 1013;
    spa.temperature = 15;

    QList<Event> ret;
    ret.reserve(route.size());

    // Go through the route and check if it is day or night
    for (const auto &pos : route) {
        spa.hour      = time_ms / MS_PER_HOUR;
        spa.minute    = (time_ms / MS_PER_MINUTE) % 60;
        spa.latitude  = pos.lat;
        spa.longitude = pos.lon;

        spa_calculate(&spa);
        ret.append({time_ms, spa.e < night_angle});
        time_ms += MS_PER_MINUTE;
        if (time_ms >= MS_PER_DAY) time_ms = time_ms % MS_PER_DAY;
    }

    return ret;
}
