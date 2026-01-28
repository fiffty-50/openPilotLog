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
#ifndef GREATCIRCLETRACK_H
#define GREATCIRCLETRACK_H

#include "latlon.h"
#include <QList>

/*!
 * \brief Utility class for great-circle distance and track calculations.
 *
 * Provides static functions to compute the angular distance between two
 * geographic coordinates and to generate intermediate points along the
 * great-circle path between them.
 *
 * All coordinates are expected to be in degrees.
 */
class GreatCircleTrack {
  public:
    GreatCircleTrack() = delete;

    /*!
     * \brief Computes a great-circle track between two geographic positions.
     *
     * \param departure_deg   Departure coordinates in degrees.
     * \param destination_deg Destination coordinates in degrees.
     * \param duration_ms     Total duration in milliseconds.
     * \param time_between_steps Time resolution in milliseconds (default: one minute).
     *
     * \return Vector of points along the great-circle path.
     *
     * \note If only one step exists, the destination is returned.
     * \note If departure equals destination, all points are identical.
     */
    static std::vector<LatLon> greatCircleTrack(const LatLon &departure_deg,
                                                const LatLon &destination_deg, int duration_ms,
                                                int time_between_steps = ONE_MINUTE_MS);

    /*!
     * \brief Computes the great-circle distance between two geographic points.
     *
     * Uses the haversine formula to calculate the central angle between two points
     * on a sphere. The result represents the angular distance along the surface of
     * the sphere.
     *
     * \return Great-circle distance as a central angle, in radians.
     *
     * \note All latitude and longitude values **must be in degrees**.
     */
    static constexpr double greatCircleDistanceRadians(const LatLon &departure_deg,
                                                       const LatLon &destination_deg);

  private:
    static constexpr double deg_to_rad(double deg) { return deg * M_PI / 180.0; }
    static constexpr double rad_to_deg(double rad) { return rad * 180.0 / M_PI; }
    static constexpr int ONE_MINUTE_MS = 60'000;
};

#endif // GREATCIRCLETRACK_H
