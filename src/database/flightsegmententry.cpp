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

FlightSegmentEntry::FlightSegmentEntry(int segment_id, const RowData_T &row_data)
    : Row(OPL::DbTable::v2FlightSegments, segment_id, row_data, &FIELDS)
{
}

FlightSegmentEntry::FlightSegmentEntry(int flight_id, int start_ms, int end_ms, bool is_ifr,
                                       bool is_simulated_ifr, bool is_night, bool is_multi_pilot,
                                       bool is_pilot_flying, const QString &pilot_function)
    : Row(OPL::DbTable::v2FlightSegments, &FIELDS)
{
    m_rowData.insert(FLIGHT_ID, flight_id);
    m_rowData.insert(START_MS, start_ms);
    m_rowData.insert(END_MS, end_ms);
    m_rowData.insert(IS_IFR, is_ifr);
    m_rowData.insert(IS_SIM_IFR, is_simulated_ifr);
    m_rowData.insert(IS_NIGHT, is_night);
    m_rowData.insert(IS_MULTI_PILOT, is_multi_pilot);
    m_rowData.insert(IS_PILOT_FLYING, is_pilot_flying);
    m_rowData.insert(PILOT_FUNCTION, pilot_function);
}

FlightSegmentEntry::FlightSegmentEntry(int flight_id, int start_ms, int end_ms)
    : Row(OPL::DbTable::v2FlightSegments, &FIELDS)
{
    m_rowData.insert(FLIGHT_ID, flight_id);
    m_rowData.insert(START_MS, start_ms);
    m_rowData.insert(END_MS, end_ms);
}

bool FlightSegmentEntry::isValid() const
{
    bool allValid = true;

    // must have a flight_id associated with it
    allValid &= m_rowData.value(FLIGHT_ID).toInt() > 0;

    // times must be within range
    allValid &= OPL::Time::isValidTimeOfDay(m_rowData.value(START_MS).toInt());
    allValid &= OPL::Time::isValidTimeOfDay(m_rowData.value(END_MS).toInt());

    // Pilot function must be valid
    allValid &=
        GLOBALS->getPilotFunctions()->values().contains(m_rowData.value(PILOT_FUNCTION).toString());

    return allValid;
}

bool FlightSegmentEntry::setStartMs(int start_ms)
{
    if (!OPL::Time::isValidTimeOfDay(start_ms)) {
        return false;
    }
    m_rowData.insert(START_MS, start_ms);
    return true;
}

bool FlightSegmentEntry::setEndMs(int end_ms)
{
    if (!OPL::Time::isValidTimeOfDay(end_ms)) {
        return false;
    }
    m_rowData.insert(END_MS, end_ms);
    return true;
}

void FlightSegmentEntry::setIsIfr(bool is_ifr) { m_rowData.insert(IS_IFR, is_ifr); }

void FlightSegmentEntry::setIsSimIfr(bool is_sim_ifr) { m_rowData.insert(IS_SIM_IFR, is_sim_ifr); }

void FlightSegmentEntry::setIsNight(bool is_night) { m_rowData.insert(IS_NIGHT, is_night); }

void FlightSegmentEntry::setIsMultiPilot(bool is_multi_pilot)
{
    m_rowData.insert(IS_MULTI_PILOT, is_multi_pilot);
}

void FlightSegmentEntry::setIsPilotFlying(bool is_pilot_flying)
{
    m_rowData.insert(IS_PILOT_FLYING, is_pilot_flying);
}

bool FlightSegmentEntry::setPilotFunction(const QString &pilot_function)
{
    if (!GLOBALS->getPilotFunctions()->values().contains(pilot_function)) {
        return false;
    }

    m_rowData.insert(PILOT_FUNCTION, pilot_function);
    return true;
}

} // namespace OPL
