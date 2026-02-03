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
#include "airportgeographicalinfo.h"
#include "src/database/database.h"

AirportGeographicalInfo::AirportGeographicalInfo(QObject *parent) : QObject{parent}
{
    refresh();

    connect(DB, &OPL::Database::dataBaseUpdated, this, [this](OPL::DbTable table) {
        if (table == OPL::DbTable::v2Airports) refresh();
    });
}

void AirportGeographicalInfo::refresh()
{
    int size = m_airportGeoMap.size() == 0 ? 6000 : m_airportGeoMap.size();
    m_airportGeoMap.clear();
    m_airportGeoMap.reserve(size);

    QSqlQuery query;
    query.exec(QStringLiteral("SELECT * FROM airports"));

    while (query.next()) {
        int airportId = query.value(0).toInt();
        double lat    = query.value(2).toDouble();
        double lon    = query.value(3).toDouble();
        QString tz    = query.value(4).toString();

        m_airportGeoMap.insert(airportId, GeoData{lat, lon, tz});
    }
}

double AirportGeographicalInfo::latitude(int airport_id) const
{
    return m_airportGeoMap.value(airport_id).lat;
}

double AirportGeographicalInfo::longitude(int airport_id) const
{
    return m_airportGeoMap.value(airport_id).lon;
}

QString AirportGeographicalInfo::timezone(int airport_id) const
{
    return m_airportGeoMap.value(airport_id).tz;
}

bool AirportGeographicalInfo::exists(int airport_id) const
{
    return m_airportGeoMap.contains(airport_id);
}
LatLon AirportGeographicalInfo::coordinates(int airport_id) const
{
    return {m_airportGeoMap.value(airport_id).lat, m_airportGeoMap.value(airport_id).lon};
}

LatLon AirportGeographicalInfo::coordinates(const QString &icao_code)
{
    int id = airportData->idFromIcao(icao_code);
    return {m_airportGeoMap.value(id).lat, m_airportGeoMap.value(id).lon};
}
