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
#include "movemententry.h"

namespace OPL {

MovementEntry::MovementEntry(int event_id, int airport_id, bool is_landing, bool is_night,
                             bool is_autoland)
    : Row(OPL::DbTable::MovementEvents, &FIELDS)
{
    assert(event_id > 0);
    m_rowData.insert(EVENT_ID, event_id);

    if (airport_id > 0) {
        m_rowData.insert(AIRPORT_ID, airport_id);
    }

    m_rowData.insert(IS_LANDING, is_landing);
    m_rowData.insert(IS_NIGHT, is_night);
    m_rowData.insert(IS_AUTOLAND, is_autoland);
}

MovementEntry::MovementEntry(int row_id, const RowData_T &row_data)
    : Row(OPL::DbTable::MovementEvents, row_id, row_data, &FIELDS)
{
}

bool MovementEntry::isValid() const
{
    bool allValid = true;

    // only airport_id can be NULL
    allValid &= m_rowData.values().count(OPL::NullData) < 2;

    // event_id must be set
    allValid &= m_rowData.value(EVENT_ID).toInt() > 0;

    return allValid;
}

bool MovementEntry::isLanding() const { return m_rowData.value(IS_LANDING).toBool(); }

bool MovementEntry::isNight() const { return m_rowData.value(IS_NIGHT).toBool(); }

bool MovementEntry::isAutoland() const { return m_rowData.value(IS_AUTOLAND).toBool(); }

int MovementEntry::airportId() const { return m_rowData.value(AIRPORT_ID).toInt(); }

} // namespace OPL
