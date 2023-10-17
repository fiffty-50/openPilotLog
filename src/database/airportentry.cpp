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

AirportEntry::AirportEntry()
    : Row(DbTable::Airports, 0)
{}

AirportEntry::AirportEntry(const RowData_T &row_data)
    : Row(DbTable::Airports, 0, row_data)
{}

AirportEntry::AirportEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Airports, row_id, row_data)
{}

const QString AirportEntry::getTableName() const
{
    return TABLE_NAME;
}

const QString AirportEntry::getIataCode() const
{
    return getData().value(IATA).toString();
}

const QString AirportEntry::getIcaoCode() const
{
    return getData().value(ICAO).toString();
}

} // namespace OPL
