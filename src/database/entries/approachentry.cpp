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
#include "approachentry.h"
#include "src/database/cache/approachtypeinfo.h"

namespace OPL {

ApproachEntry::ApproachEntry(int event_id, int approach_type_id)
    : Row(OPL::DbTable::ApproachEvents, &FIELDS)
{
    m_rowData.insert(EVENT_ID, event_id);
    m_rowData.insert(APP_TYPE, approach_type_id);
}

ApproachEntry::ApproachEntry(int row_id, const RowData_T &row_data)
    : Row(OPL::DbTable::ApproachEvents, row_id, row_data, &FIELDS)
{
}

bool ApproachEntry::isValid() const
{
    bool allValid = true;

    // Event ID must be valid
    allValid &= m_rowData.value(EVENT_ID).toInt() > 0;

    // Approach Type must not be empty
    allValid &= m_rowData.value(APP_TYPE).toInt() > 0;

    return allValid;
}

bool ApproachEntry::setEventId(int event_id)
{
    if (event_id == 0) {
        return false;
    }

    m_rowData.insert(EVENT_ID, event_id);
    return true;
}

bool ApproachEntry::setApproachType(int approach_id)
{
    if(approachData->exists(approach_id)) {
        m_rowData.insert(APP_TYPE, approach_id);
        return true;
    } else {
        return false;
    }
}

} // namespace OPL
