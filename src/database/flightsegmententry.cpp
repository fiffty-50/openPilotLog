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
#include "flightsegmententry.h"
#include "src/classes/time.h"

namespace OPL {

FlightSegmentEntry::FlightSegmentEntry(int flight_id, const RowData_T &row_data)
    : Row(OPL::DbTable::v2FlightSegments, flight_id, row_data, &FIELDS)
{
}

bool FlightSegmentEntry::isValid() const
{
    bool allValid = true;

    // must have a flight_id associated with it
    allValid &= m_rowData.value(FLIGHT_ID).toInt() > 0;

    // times must be within range
    allValid &= OPL::Time::isValidTimeOfDay(m_rowData.value(START_MS).toInt());
    allValid &= OPL::Time::isValidTimeOfDay(m_rowData.value(END_MS).toInt());

    return allValid;
}

} // namespace OPL
