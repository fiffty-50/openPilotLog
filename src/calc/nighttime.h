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

#include "src/opl.h"
#include <QTime>
#include <QVector>
// extern "C" {
// #include "src/calc/spa.c"
// #include "src/calc/spa.h"
// }

/**
 * @class NightTime
 * @brief Provides functionality to calculate night time during a flight.
 *
 * This class calculates the portion of a flight that occurs during night conditions
 * using a one-minute resolution along the great circle route between two airports.
 * Night is defined as periods when the sun's elevation is below a configurable threshold.
 *
 * Assumptions:
 * - Standard cruising altitude is 11 km (~FL360).
 * - Night angle (solar elevation threshold) is configurable to comply with different
 *   aviation regulations or definitions of night.
 */
class NightTime {
  public:
    /**
     * @brief Calculates the total night time for a flight between two airports.
     *
     * This function divides the flight into one-minute intervals along the great circle
     * route. At each step, it evaluates the solar elevation at the current position and
     * counts minutes where the elevation is below the specified night_angle.
     *
     * @param dept_airport_id The ID of the departure airport.
     * @param dest_airport_id The ID of the destination airport.
     * @param departure_time_ms Departure time in milliseconds since start of day.
     * @param flight_time_minutes Total flight duration in minutes.
     * @param night_angle Solar elevation threshold (in degrees) below which it is considered night.
     * @return Total night time in milliseconds.
     */
    static int calculateNightTime(int dept_airport_id, int dest_airport_id, int departure_time_ms,
                                  int flight_time_minutes, double night_angle)
    {
        Q_UNIMPLEMENTED();
        return -1;
    }

    /**
     * @brief Determines if a specific airport is experiencing night at a given time.
     *
     * Evaluates the solar elevation at the airport's location for the specified time.
     * Returns true if the elevation is below the provided night_angle.
     *
     * @param airport_id The ID of the airport to check.
     * @param time_ms_since_start_of_day Time in milliseconds since start of day.
     * @param night_angle Solar elevation threshold (in degrees) below which it is considered night.
     * @return True if the location is in night conditions, false otherwise.
     */
    static bool isNight(int airport_id, int time_ms_since_start_of_day, double night_angle)
    {
        Q_UNIMPLEMENTED();
        return false;
    }

  private:
    static double solarElevation(int time_ms_since_start_of_day, double lat, double lon,
                                 double sin_lat = NAN, double cos_lat = NAN);
};

#endif // NIGHTTIME_H
