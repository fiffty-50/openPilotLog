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
#include "greatcircletrack.h"
#include "src/calc/latlon.h"

std::vector<LatLon> GreatCircleTrack::greatCircleTrack(const LatLon &departure_deg,
                                                       const LatLon &destination_deg,
                                                       int duration_ms, int time_between_steps)
{
    // Calculate one-minute steps along the route and put their coordinates in a list
    int num_steps = duration_ms / time_between_steps;
    std::vector<LatLon> route;

    // handle special cases:
    // ultra short flights
    if (num_steps < 2) {
        return {destination_deg};
    }
    // local flight
    if (departure_deg == destination_deg) {
        route.assign(num_steps, departure_deg);
        return route;
    }

    double d = GreatCircleTrack::greatCircleDistanceRadians(departure_deg, destination_deg);

    double lat1 = deg_to_rad(departure_deg.lat);
    double lon1 = deg_to_rad(departure_deg.lon);
    double lat2 = deg_to_rad(destination_deg.lat);
    double lon2 = deg_to_rad(destination_deg.lon);

    // calculate
    const double sin_d    = std::sin(d);
    const double cos_lat1 = std::cos(lat1);
    const double cos_lat2 = std::cos(lat2);
    route.reserve(num_steps);
    for (int i = 0; i < num_steps; ++i) {
        const double f = double(i) / (num_steps - 1);

        const double A = std::sin((1 - f) * d) / sin_d;
        const double B = std::sin(f * d) / sin_d;

        const double x = A * cos_lat1 * std::cos(lon1) + B * cos_lat2 * std::cos(lon2);
        const double y = A * cos_lat1 * std::sin(lon1) + B * cos_lat2 * std::sin(lon2);
        const double z = A * std::sin(lat1) + B * std::sin(lat2);

        const double lat = std::atan2(z, std::sqrt(x * x + y * y));
        const double lon = std::atan2(y, x);

        route.emplace_back(rad_to_deg(lat), rad_to_deg(lon));
    }

    return route;
}

constexpr double GreatCircleTrack::greatCircleDistanceRadians(const LatLon &departure_deg,
                                                              const LatLon &destination_deg)
{
    const double lat1 = deg_to_rad(departure_deg.lat);
    const double lon1 = deg_to_rad(departure_deg.lon);
    const double lat2 = deg_to_rad(destination_deg.lat);
    const double lon2 = deg_to_rad(destination_deg.lon);

    const double dlat = lat2 - lat1;
    const double dlon = lon2 - lon1;

    const double sin_dlat = std::sin(dlat * 0.5);
    const double sin_dlon = std::sin(dlon * 0.5);

    double a = sin_dlat * sin_dlat + std::cos(lat1) * std::cos(lat2) * sin_dlon * sin_dlon;

    a = std::min(1.0, a);

    // return distance in radians.
    return 2.0 * std::asin(std::sqrt(a));
}
