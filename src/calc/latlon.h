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
#ifndef LATLON_H
#define LATLON_H

#include "src/database/cache/airportinfo.h"
#include <QString>

/*!
 * \brief Encapsulates two doubles latitude and longitude representing a geopraphical coordinate.
 */
struct LatLon {
    double lat;
    double lon;

    /*!
     * \brief Compares two coordinates for exact equality.
     */
    constexpr bool operator==(const LatLon &other) const noexcept
    {
        return lat == other.lat && lon == other.lon;
    }

    /*!
     * \brief Compares two coordinates using a tolerance.
     * \param epsilon Maximum allowed difference.
     */
    constexpr bool almostEquals(const LatLon &other, double epsilon = 1e-7) const noexcept
    {
        return std::abs(lat - other.lat) < epsilon && std::abs(lon - other.lon) < epsilon;
    }

    /*!
     * \brief Overloads the QString operator for printing -> "[lat, lon]"
     */
    operator QString() const { return QString("[%1,%2]").arg(lat).arg(lon); }
};

#endif
