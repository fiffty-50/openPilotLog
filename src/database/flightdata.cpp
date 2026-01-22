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
#include "flightdata.h"
#include "src/database/flightsegmententry.h"
namespace OPL {

FlightData::FlightData(const LogEntry &log_entry, const FlightLogEntry &flight_entry,
                       const QList<FlightSegmentEntry> &segments,
                       const QList<MovementEntry> &movements)
    : m_log_entry(log_entry), m_flight_entry(flight_entry), m_segments(segments),
      m_movements(movements)
{
    assert(m_segments.size() > 0);
}

int FlightData::getTakeOffCount() const
{
    if (m_movements.isEmpty()) {
        return 0;
    }

    int count = 0;
    for (const auto &movement : m_movements) {
        if (!movement.isLanding()) {
            count++;
        }
    }
    return count;
}

int FlightData::getLandingCount() const
{
    if (m_movements.isEmpty()) {
        return 0;
    }

    int count = 0;
    for (const auto &movement : m_movements) {
        if (movement.isLanding()) {
            count++;
        }
    }
    return count;
}

QTime FlightData::getBlockTime() const
{
    int block_time_ms = 0;

    for (const auto &segment : m_segments) {
        block_time_ms += (segment.endMs() - segment.startMs());
    }

    return QTime::fromMSecsSinceStartOfDay(block_time_ms);
}

QTime FlightData::getNightTime() const
{
    int night_time = 0;
    for (const auto &segment : m_segments) {
        if (segment.isNight()) {
            night_time += (segment.endMs() - segment.startMs());
        }
    }

    return QTime::fromMSecsSinceStartOfDay(night_time);
}

QString FlightData::pilotFunction() const
{
    if (m_segments.isEmpty()) {
        return QString();
    }
    return m_segments.first().pilotFunction();
}

bool FlightData::isPilotFlying() const
{
    if (m_segments.isEmpty()) {
        return false;
    }
    return m_segments.first().isPilotFlying();
}

bool FlightData::isIfr() const
{
    if (m_segments.isEmpty()) {
        return false;
    }
    return m_segments.first().isIfr();
}

bool FlightData::isSimulatedIfr() const
{
    if (m_segments.isEmpty()) {
        return false;
    }
    return m_segments.first().isSimulatedIfr();
}

bool FlightData::isMultiPilot() const
{
    if (m_segments.isEmpty()) {
        return false;
    }
    return m_segments.first().isMultiPilot();
}

} // namespace OPL
