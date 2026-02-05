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
#include "src/database/approachentry.h"
#include "src/database/flightlogentry.h"
#include "src/database/flightsegmententry.h"
#include "src/database/movemententry.h"
#include "src/database/pilotinfo.h"
#include "src/database/tailregistrationsinfo.h"
#include "src/opl.h"
#include <QObject>
#include <utility>

namespace OPL {

// Validation
bool FlightDataBuilder::validate()
{
    LOG << QStringLiteral("Validating flight data...");
    bool allValid = true;
    m_errors.clear();
    // Mandatory Log data
    if (m_date_jd == OPL::NullData) {
        m_errors << QStringLiteral("Invalid Date.");
        allValid = false;
    }

    // mandatory Flight Data
    if (m_departure_id == OPL::NullData) {
        m_errors << QStringLiteral("Invalid Departure id.");
        allValid = false;
    }
    if (m_destination_id == OPL::NullData) {
        m_errors << QStringLiteral("Invalid Destination id.");
        allValid = false;
    }
    if (m_time_off_ms == OPL::NullData) {
        m_errors << QStringLiteral("Invalid Time Off Blocks.");
        allValid = false;
    }
    if (m_time_on_ms == OPL::NullData) {
        m_errors << QStringLiteral("Invalid Time On Blocks.");
        allValid = false;
    }
    if (m_pic_id == OPL::NullData) {
        m_errors << QStringLiteral("Invalid pic id.");
        allValid = false;
    }
    if (m_tail_id == OPL::NullData) {
        m_errors << QStringLiteral("Invalid tail id.");
        allValid = false;
    }

    // at least one segment must be present
    if (m_segment_data.isEmpty()) {
        m_errors << QStringLiteral("Invalid Flight Segment Data.");
        allValid = false;
    }
    LOG << QStringLiteral("Mandatory flight data valid.");

    // run some sanity checks on the data that has been collected
    if (m_pic_id == m_second_pilot_id_opt) {
        m_errors << QStringLiteral("PIC and SIC are the same.");
        allValid &= false;
    }

    bool owner_is_pic = m_pic_id == OPL::LOGBOOK_OWNER_ID;
    if (m_pilot_function_opt) {
        auto function = m_pilot_function_opt.value();
        bool function_is_pic_compatible =
            (function == OPL::PilotFunction::PIC || function == OPL::PilotFunction::FI);
        if (owner_is_pic != function_is_pic_compatible) {
            m_errors << QStringLiteral("PIC and Pilot Function are inconsistent.");
            allValid &= false;
        }
    }

    LOG << QStringLiteral("All cheks passed.");

    return allValid;
}

// Mandatory Data setters

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

// Optional Data Setters
void FlightDataBuilder::addRemarks(const QString &remarks) { m_remarks_opt = remarks; }

bool FlightDataBuilder::addSecondPilot(int pilot_id)
{
    if (!pilotsData->exists(pilot_id)) return false;
    m_second_pilot_id_opt = pilot_id;

    return true;
}

void FlightDataBuilder::addFlightNumber(const QString &flight_number)
{
    if (!flight_number.isEmpty()) m_flight_number_opt = flight_number;
}

void FlightDataBuilder::addPilotFunction(OPL::PilotFunction function)
{
    m_pilot_function_opt = function;
}

// Entry creation
LogEntry FlightDataBuilder::logEntry() const
{
    LogEntry entry;
    entry.setEventType(EVENT_TYPE);
    entry.setDate(m_date_jd);
    if (m_remarks_opt) entry.setRemarks(*m_remarks_opt);

    return entry;
}

bool FlightDataBuilder::addSegmentData(const QList<FlightSegmentBuilder::SegmentData> &segments)
{
    if (segments.size() < 1) return false;
    m_segment_data = segments;
    return true;
}

FlightLogEntry FlightDataBuilder::flightLogEntry() const
{
    FlightLogEntry entry;

    // mandatory data
    entry.setEventId(m_event_id);
    entry.setDeparture(m_departure_id);
    entry.setDestination(m_destination_id);
    entry.setTimeOffBlocks(m_time_off_ms);
    entry.setTimeOnBlocks(m_time_on_ms);
    entry.setPic(m_pic_id);
    entry.setTail(m_tail_id);

    // optional data
    if (m_second_pilot_id_opt) entry.setSecondPilot(*m_second_pilot_id_opt);
    if (m_third_pilot_id_opt) entry.setThirdPilot(*m_third_pilot_id_opt);
    if (m_fourth_pilot_id_opt) entry.setFourthPilot(*m_fourth_pilot_id_opt);
    if (m_flight_number_opt) entry.setFlightNumber(*m_flight_number_opt);

    return entry;
}

QList<FlightSegmentEntry> FlightDataBuilder::flightSegments() const
{
    if (m_flight_id < 1) return {};
    if (m_segment_data.isEmpty()) return {};

    QList<FlightSegmentEntry> ret;

    for (const auto &s : std::as_const(m_segment_data)) {
        FlightSegmentEntry entry(m_flight_id, s.start_ms, s.end_ms, s.opts);
        ret.append(entry);
    }

    return ret;
}

QList<MovementEntry> FlightDataBuilder::movements() const
{
    if (m_event_id < 1) return {};
    if (m_movement_event_data.isEmpty()) return {};

    QList<MovementEntry> ret;
    ret.reserve(m_movement_event_data.size());

    for (const auto &m : std::as_const(m_movement_event_data)) {
        ret.append(MovementEntry(m_event_id, m.airport_id, m.isLanding, m.isNight, m.isAutoland));
    }

    return ret;
}

QList<ApproachEntry> FlightDataBuilder::approaches() const
{
    if (m_event_id < 1) return {};
    if (m_approach_data.isEmpty()) return {};

    QList<ApproachEntry> ret;
    ret.reserve(m_approach_data.size());

    for (const auto &a : std::as_const(m_approach_data)) {
        ret.append(ApproachEntry(m_event_id, a.approach_type, a.airport_id));
    }

    return ret;
}

} // namespace OPL
