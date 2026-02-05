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

#include "simlogentry.h"
#include "src/classes/time.h"
#include "src/database/cache/aircrafttypesinfo.h"
#include "src/database/cache/pilotinfo.h"
#include "src/opl.h"

namespace OPL {

SimLogEntry::SimLogEntry() : Row(DbTable::v2Simulators, &FIELDS) {}

SimLogEntry::SimLogEntry(int row_id, const RowData_T &row_data)
    : Row(OPL::DbTable::v2Simulators, row_id, row_data, &FIELDS)
{
}

bool SimLogEntry::isValid() const
{
    bool allValid = true;

    // Check mandatory fields are set
    allValid &= m_rowData.value(EVENT_ID).toInt() >= 0;
    allValid &= !m_rowData.value(SIM_TYPE).toString().isEmpty();
    allValid &= m_rowData.value(DURATION).toInt() > 0;
    allValid &= OPL::Time::isValidTimeOfDay(m_rowData.value(DURATION).toInt());

    return allValid;
}

// Setters
bool SimLogEntry::setEventId(int event_id)
{
    if (event_id < 0) {
        return false;
    }
    m_rowData.insert(EVENT_ID, event_id);
    return true;
}

bool SimLogEntry::setSimType(const QString &sim_type)
{
    if (sim_type.isEmpty()) {
        return false;
    }
    m_rowData.insert(SIM_TYPE, sim_type);
    return true;
}

bool SimLogEntry::setAircraftTypeId(int aircraft_type_id)
{
    if (! aircraftTypesData->contains(aircraft_type_id)) return false;
    m_rowData.insert(AIRCRAFT_TYPE_ID, aircraft_type_id);
    return true;
}

bool SimLogEntry::setInstructorId(int instructor_id)
{
    if (!pilotsData->contains(instructor_id)) return false;
    m_rowData.insert(INSTRUCTOR_ID, instructor_id);
    return true;
}

bool SimLogEntry::setSecondPilotId(int second_pilot_id)
{
    if (!pilotsData->contains(second_pilot_id)) return false;
    m_rowData.insert(SECOND_PILOT_ID, second_pilot_id);
    return true;
}

bool SimLogEntry::setDurationMs(int duration_ms)
{
    if (duration_ms == 0 || (!OPL::Time::isValidTimeOfDay(duration_ms))) {
        return false;
    }
    m_rowData.insert(DURATION, duration_ms);
    return true;
}

// Getters

int SimLogEntry::getEventId() const { return m_rowData.value(EVENT_ID).toInt(); }

QString SimLogEntry::getSimType() const { return m_rowData.value(SIM_TYPE).toString(); }

int SimLogEntry::getAircraftTypeId() const { return m_rowData.value(AIRCRAFT_TYPE_ID).toInt(); }

int SimLogEntry::getInstructorId() const { return m_rowData.value(INSTRUCTOR_ID).toInt(); }

int SimLogEntry::getSecondPilotId() const { return m_rowData.value(SECOND_PILOT_ID).toInt(); }

int SimLogEntry::getDurationMs() const { return m_rowData.value(DURATION).toInt(); }

} // namespace OPL
