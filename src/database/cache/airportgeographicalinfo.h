
/*                                                                       \
 *openPilotLog - A FOSS Pilot Logbook Application                        \
 *Copyright (C) 2020-2026 Felix Turowsky                                 \
 *                                                                       \
 *This program is free software: you can redistribute it and/or modify   \
 *it under the terms of the GNU General Public License as published by   \
 *the Free Software Foundation, either version 3 of the License, or      \
 *(at your option) any later version.                                    \
 *                                                                       \
 *This program is distributed in the hope that it will be useful,        \
 *but WITHOUT ANY WARRANTY; without even the implied warranty of         \
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          \
 *GNU General Public License for more details.                           \
 *                                                                       \
 *You should have received a copy of the GNU General Public License      \
 *along with this program.  If not, see <https://www.gnu.org/licenses/>. \
 */
#ifndef AIRPORTGEOGRAPHICALINFO_H
#define AIRPORTGEOGRAPHICALINFO_H

#include "src/calc/latlon.h"
#include <QHash>
#include <QObject>
/*!
 * \class AirportGeographicalInfo
 * \brief Provides geographical information for airports.
 *
 * \details
 * Loads airport data from the database and allows fast lookup of
 * latitude, longitude, and timezone by airport ID.
 *
 * Access to a global instance is provided via:
 * \code
 * Q_GLOBAL_STATIC(AirportGeographicalInfo, airportGeoData)
 * \endcode
 */
class AirportGeographicalInfo : public QObject {
    Q_OBJECT
  public:
    explicit AirportGeographicalInfo(QObject *parent = nullptr);

    void inline init() { refresh(); }

    /*!
     * \brief Returns the latitude for the given airport ID.
     */
    double latitude(int airport_id) const;

    /*!
     * \brief Returns the longitude for the given airport ID.
     */
    double longitude(int airport_id) const;

    /*!
     * \brief Returns the LatLon coordinates for the given airport ID.
     */
    LatLon coordinates(int airport_id) const;

    /*!
     * \brief Returns the LatLon coordinates for the given airport ICAO code.
     * \param icao_code ICAO airport identifier.
     */
    LatLon coordinates(const QString &icao_code);

    /*!
     * \brief Returns the timezone (Olson string) for the given airport ID.
     */
    QString timezone(int airport_id) const;

    /*!
     * \brief Returns whether a row id contains in the database
     */
    bool contains(int airport_id) const;

  private:
    struct GeoData {
        double lat = 0.0;
        double lon = 0.0;
        QString tz;
    };

    QHash<int, GeoData> m_airportGeoMap;

    void refresh();
};

Q_GLOBAL_STATIC(AirportGeographicalInfo, airportGeoData)

#endif // AIRPORTGEOGRAPHICALINFO_H
