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

#include "flightlogentry.h"
#include "src/classes/time.h"
#include "src/database/cache/airportinfo.h"
#include "src/database/cache/pilotinfo.h"
#include "src/database/cache/tailregistrationsinfo.h"
#include "src/database/row.h"
#include "src/opl.h"

namespace OPL {

FlightLogEntry::FlightLogEntry() : Row(DbTable::v2Flights, &FIELDS) {}

FlightLogEntry::FlightLogEntry(int rowId, const RowData_T &rowData)
    : Row(OPL::DbTable::v2Flights, rowId, rowData, &FIELDS)
{
}

bool FlightLogEntry::isValid() const
{
    // todo run checks
    bool allValid = true;

    // Check mandatory fields are set
    allValid &= m_rowData.value(EVENT_ID).toInt() > 0;
    allValid &= m_rowData.value(DEPARTURE).toInt() > 0;
    allValid &= m_rowData.value(DESTINATION).toInt() > 0;
    allValid &= m_rowData.value(T_OFF_BLOCK).toInt() >= 0;
    allValid &= m_rowData.value(T_ON_BLOCK).toInt() >= 0;
    allValid &= m_rowData.value(PILOT_PIC).toInt() > 0;
    allValid &= m_rowData.value(TAIL).toInt() > 0;

    return allValid;
}

// Setters

bool FlightLogEntry::setEventId(int event_id)
{
    if (event_id < 0) {
        return false;
    }
    m_rowData.insert(EVENT_ID, event_id);
    return true;
}

bool FlightLogEntry::setDeparture(int airport_id)
{
    if (!airportData->contains(airport_id)) return false;
    m_rowData.insert(DEPARTURE, airport_id);
    return true;
}

bool FlightLogEntry::setDestination(int airport_id)
{
    if (!airportData->contains(airport_id)) return false;
    m_rowData.insert(DESTINATION, airport_id);
    return true;
}

bool FlightLogEntry::setTimeOffBlocks(int time_ms)
{
    if (!OPL::Time::isValidTimeOfDay(time_ms)) {
        return false;
    }

    m_rowData.insert(T_OFF_BLOCK, time_ms);
    return true;
}

bool FlightLogEntry::setTimeOnBlocks(int time_ms)
{
    if (!OPL::Time::isValidTimeOfDay(time_ms)) {
        return false;
    }

    m_rowData.insert(T_ON_BLOCK, time_ms);
    return true;
}

bool FlightLogEntry::setPic(int pilot_id)
{
    if (!pilotsData->contains(pilot_id)) return false;
    m_rowData.insert(PILOT_PIC, pilot_id);
    return true;
}

bool FlightLogEntry::setSecondPilot(int pilot_id)
{
    if (!pilotsData->contains(pilot_id)) return false;
    m_rowData.insert(PILOT_TWO, pilot_id);
    return true;
}

bool FlightLogEntry::setThirdPilot(int pilot_id)
{
    if (!pilotsData->contains(pilot_id)) return false;
    m_rowData.insert(PILOT_THREE, pilot_id);
    return true;
}

bool FlightLogEntry::setFourthPilot(int pilot_id)
{
    if (!pilotsData->contains(pilot_id)) return false;
    m_rowData.insert(PILOT_FOUR, pilot_id);
    return true;
}

bool FlightLogEntry::setTail(int tail_id)
{
    if (!tailsData->contains(tail_id)) return false;
    m_rowData.insert(TAIL, tail_id);
    return true;
}

bool FlightLogEntry::setFlightNumber(const QString &flight_number)
{
    m_rowData.insert(FLIGHT_NUMBER, flight_number);
    return true;
}

// Getters
int FlightLogEntry::getEventId() const { return m_rowData.value(EVENT_ID).toInt(); }

int FlightLogEntry::getDepartureId() const { return m_rowData.value(DEPARTURE).toInt(); }

int FlightLogEntry::getDestinationId() const { return m_rowData.value(DESTINATION).toInt(); }

int FlightLogEntry::getTimeOffBlocksMs() const { return m_rowData.value(T_OFF_BLOCK).toInt(); }

int FlightLogEntry::getTimeOnBlocksMs() const { return m_rowData.value(T_ON_BLOCK).toInt(); }

int FlightLogEntry::getPicId() const { return m_rowData.value(PILOT_PIC).toInt(); }

int FlightLogEntry::getSecondPilotId() const { return m_rowData.value(PILOT_TWO).toInt(); }

int FlightLogEntry::getThirdPilotId() const { return m_rowData.value(PILOT_THREE).toInt(); }

int FlightLogEntry::getFourthPilotId() const { return m_rowData.value(PILOT_FOUR).toInt(); }

int FlightLogEntry::getTailId() const { return m_rowData.value(TAIL).toInt(); }

QString FlightLogEntry::getFlightNumber() const
{
    return m_rowData.value(FLIGHT_NUMBER).toString();
}

} // namespace OPL
