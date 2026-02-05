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
#include "airportentry.h"

namespace OPL {

AirportEntry::AirportEntry() : Row(DbTable::v2Airports, &FIELDS) {}

AirportEntry::AirportEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::v2Airports, row_id, row_data, &FIELDS)
{
}

bool AirportEntry::isValid() const
{
    // verify airport name is set
    return !(m_rowData.value(NAME).toString().isEmpty());
}

bool AirportEntry::setAirportName(const QString &input)
{
    if (input.isEmpty()) return false;
    m_rowData.insert(NAME, input);
    return true;
}

bool AirportEntry::setLatitude(double input)
{
    if (!(input >= -90.0 && input <= 90.0)) return false;

    m_rowData.insert(LATITUDE, input);
    return true;
}

bool AirportEntry::setLongitude(double input)
{
    if (!(input >= -180.0 && input <= 180.0)) return false;

    m_rowData.insert(LONGITUDE, input);
    return true;
}

bool AirportEntry::setTimezone(const QString &input)
{
    if (!QTimeZone::availableTimeZoneIds().contains(input)) return false;

    m_rowData.insert(TZ_OLSON, input);
    return true;
}

QString AirportEntry::getAirportName() const { return getData().value(NAME).toString(); }

QString AirportEntry::getTimezone() const { return getData().value(TZ_OLSON).toString(); }

double AirportEntry::getLatitude() const { return getData().value(LATITUDE).toDouble(); }

double AirportEntry::getLongitude() const { return getData().value(LONGITUDE).toDouble(); }

} // namespace OPL
