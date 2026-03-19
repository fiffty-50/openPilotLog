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
#include "src/database/database.h"
#include "src/database/entries/approachentry.h"
#include "src/database/entries/flightsegmententry.h"
namespace OPL {

FlightData::FlightData(const LogEntry &log_entry, const FlightLogEntry &flight_entry,
                       const QList<FlightSegmentEntry> &segments,
                       const QList<MovementEntry> &movements,
                       const QList<ApproachEntry> &approaches)
    : m_log_entry(log_entry), m_flight_entry(flight_entry), m_segments(segments),
      m_movements(movements), m_approaches(approaches)
{
    if (m_segments.size() < 1) DEB << "Error: flight data contains no flight segments.";
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
        block_time_ms += (segment.getEndMs() - segment.getStartMs());
    }

    return QTime::fromMSecsSinceStartOfDay(block_time_ms);
}

QTime FlightData::getNightTime() const
{
    int night_time = 0;
    for (const auto &segment : m_segments) {
        if (segment.isNight()) {
            night_time += (segment.getEndMs() - segment.getStartMs());
        }
    }

    return QTime::fromMSecsSinceStartOfDay(night_time);
}

QString FlightData::pilotFunction() const
{
    if (m_segments.isEmpty()) {
        return QString();
    }
    return m_segments.first().getPilotFunction();
}

int FlightData::getFirstApproachId() const
{
    if(m_approaches.isEmpty()) return -1;
    else return m_approaches.first().getApproachId();
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

QList<RowData_T> FlightData::getData(DataType data_type, int event_id)
{
    QString statement;

    switch (data_type) {
    case Segments:
        statement = QStringLiteral("SELECT * FROM flight_segments WHERE flight_id = ?");
        break;
    case Movements:
        statement = QStringLiteral("SELECT * FROM movement_events WHERE event_id = ?");
        break;
    case Approaches:
        statement = QStringLiteral("SELECT * FROM approach_events WHERE event_id = ?");
        break;
    }

    QSqlQuery q;
    q.prepare(statement);
    q.addBindValue(event_id);
    q.setForwardOnly(true);

    if (!q.exec()) {
        DEB << "SQL error: " << q.lastError().text();
        DEB << "Statement: " << q.lastQuery();
        return {};
    }

    QList<RowData_T> result;

    while (q.next()) {
        auto r = q.record();
        if (r.count() == 0) continue;

        RowData_T entry_data;
        for (int i = 0; i < r.count(); i++) {
            if (!r.value(i).isNull()) {
                entry_data.insert(r.fieldName(i), r.value(i));
            }
        }

        if (!entry_data.isEmpty()) {
            result.append(entry_data);
        }
    }

    return result;
}

// database
std::optional<FlightData> FlightData::getFlightData(int event_id)
{
    const auto log_data = DB->getRowData(OPL::DbTable::LogEvents, event_id);
    if (log_data.isEmpty()) {
        LOG << QStringLiteral("Unable to retreive data - no log event found for event_id: ")
            << event_id;
        return std::nullopt;
    }

    // Get Log Event
    const auto log_entry = OPL::LogEntry(event_id, log_data);

    // Get FlightLogEntry
    const auto flight_data = DB->getRowData(DbTable::Flights, QStringLiteral("event_id"), event_id);
    if (flight_data.isEmpty()) {
        LOG << QStringLiteral("Unable to retreive data - no flight found for event_id: ")
            << event_id;
        return std::nullopt;
    }
    const auto flight_entry =
        OPL::FlightLogEntry(flight_data.value(QStringLiteral("flight_id")).toInt(), flight_data);

    // get Flight Segment Data
    QList<FlightSegmentEntry> segments;
    for (const auto &data : getData(Segments, event_id)) {
        segments.append(OPL::FlightSegmentEntry(event_id, data));
    }
    if (segments.isEmpty()) {
        LOG << QStringLiteral("Unable to retreive data - no segment data found for event_id: ")
            << event_id;
        return std::nullopt;
    }

    // get Movement Events
    QList<MovementEntry> movements;
    for (const auto &data : getData(Movements, event_id)) {
        movements.append(OPL::MovementEntry(event_id, data));
    }

    // get Approach Data
    QList<ApproachEntry> approaches;
    for (const auto &data : getData(Approaches, event_id)) {
        approaches.append(OPL::ApproachEntry(event_id, data));
    }

    // Create and Return the FlightData object - movements may be empty but it is not required
    return FlightData(log_entry, flight_entry, segments, movements, approaches);
}

} // namespace OPL
