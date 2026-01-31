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
#include "flightdatabuilder.h"
#include "src/classes/date.h"
#include "src/classes/time.h"
#include "src/database/airportinfo.h"
#include "src/database/flightlogentry.h"
#include "src/database/pilotinfo.h"
#include "src/database/tailregistrationsinfo.h"
#include "src/opl.h"
#include <optional>

namespace OPL {

bool FlightDataBuilder::addMovement(int airport_id, bool is_landing, bool is_night,
                                    bool is_autoland)
{
    if (airport_id < 1) {
        DEB << "No airport_id provided for movement.";
        return false;
    }

    m_movement_event_data.append({airport_id, is_landing, is_night, is_autoland});
    return true;
}

bool FlightDataBuilder::addDate(int date_jd)
{
    if (!OPL::Date::julianDayIsValid(date_jd)) return false;

    m_date_jd = date_jd;

    return true;
}

bool FlightDataBuilder::addDepartureLocation(int departure_id)
{
    if (departure_id < 1) return false;
    if (!airportData->exists(departure_id)) return false;

    m_departure_id = departure_id;

    return true;
}

bool FlightDataBuilder::addDestinationLocation(int destination_id)
{
    if (destination_id < 1) return false;
    if (!airportData->exists(destination_id)) return false;

    m_destination_id = destination_id;

    return true;
}

bool FlightDataBuilder::addTimeOffBlocks(int time_ms)
{
    if (!OPL::Time::isValidTimeOfDay(time_ms)) return false;

    m_time_off_ms = time_ms;

    return true;
}
bool FlightDataBuilder::addTimeOnBlocks(int time_ms)
{
    if (!OPL::Time::isValidTimeOfDay(time_ms)) return false;

    m_time_on_ms = time_ms;

    return true;
}

bool FlightDataBuilder::addPic(int pilot_id)
{
    if (!pilotsData->exists(pilot_id)) return false;
    m_pic_id = pilot_id;

    return true;
}
bool FlightDataBuilder::addTail(int tail_id)
{
    if (!tailsData->exists(tail_id)) return false;
    m_tail_id = tail_id;

    return true;
}
bool FlightDataBuilder::isValid() const
{
    if (m_date_jd == OPL::NullData) {
        DEB << "Invalid Date.";
        return false;
    }
    if (m_departure_id == OPL::NullData) {
        DEB << "Invalid Departure id.";
        return false;
    }
    if (m_destination_id == OPL::NullData) {
        DEB << "Invalid Destinoation id.";
        return false;
    }
    if (m_time_off_ms == OPL::NullData) {
        DEB << "Invalid Time Off Blocks.";
        return false;
    }
    if (m_time_on_ms == OPL::NullData) {
        DEB << "Invalid Time On Blocks.";
        return false;
    }
    if (m_pic_id == OPL::NullData) {
        DEB << "Invalid pic id.";
        return false;
    }
    if (m_tail_id == OPL::NullData) {
        DEB << "Invalid tail id.";
        return false;
    }
    return true;
}

// Optional Entries
void FlightDataBuilder::addRemarks(const QString &remarks) { m_remarks = remarks; }

// Entry creation

std::optional<LogEntry> FlightDataBuilder::createLogEntry()
{
    if (!OPL::Date::julianDayIsValid(m_date_jd)) return std::nullopt;

    LogEntry entry;
    if (!entry.setEventType(EVENT_TYPE)) return std::nullopt;
    if (!entry.setDate(m_date_jd)) return std::nullopt;
    entry.setRemarks(m_remarks);

    return entry;
}

std::optional<FlightLogEntry> FlightDataBuilder::createFlightLogEntry()
{
    if (m_event_id < 1) return std::nullopt;

    FlightLogEntry entry;
    if (!entry.setEventId(m_event_id)) return std::nullopt;
    if (!entry.setDeparture(m_departure_id)) return std::nullopt;
    if (!entry.setDestination(m_destination_id)) return std::nullopt;
    if (!entry.setTimeOffBlocks(m_time_off_ms)) return std::nullopt;
    if (!entry.setTimeOnBlocks(m_time_on_ms)) return std::nullopt;
    if (!entry.setPic(m_pic_id)) return std::nullopt;
    if (!entry.setTail(m_tail_id)) return std::nullopt;

    entry.setSecondPilot(m_second_pilot_id);
    entry.setThirdPilot(m_third_pilot_id);
    entry.setFourthPilot(m_fourth_pilot_id);
    entry.setFlightNumber(m_flight_number);

    return entry;
}

} // namespace OPL
