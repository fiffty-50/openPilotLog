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
#ifndef FLIGHTDATABUILDER_H
#define FLIGHTDATABUILDER_H

#include "src/database/approachentry.h"
#include "src/database/flightlogentry.h"
#include "src/database/flightsegmententry.h"
#include "src/database/logentry.h"
#include "src/database/movemententry.h"
#include "src/gui/verification/flightsegmentbuilder.h"
#include <optional>

namespace OPL {

class FlightDataBuilder {
  public:
    FlightDataBuilder() = default;

    struct FlightData {
        LogEntry logEntry;
        FlightLogEntry flightEntry;
        QList<FlightSegmentEntry> segments;
        QList<MovementEntry> movements;
        QList<ApproachEntry> approaches;
    };

    bool validate();
    /*!
     * \brief Returns a list of errors that ocurred when trying to compile the flight data.
     * \note This list is empty before validate() has been called at least once.
     */
    QStringList errors() const { return m_errors; }

    // mandatory data
    bool addDate(int date_jd);
    bool addDepartureLocation(int departure_id);
    bool addDestinationLocation(int destination_id);
    bool addTimeOffBlocks(int time_ms);
    bool addTimeOnBlocks(int time_ms);
    bool addPic(int pilot_id);
    bool addTail(int tail_id);

    bool addSegmentData(const QList<FlightSegmentBuilder::SegmentData> &segments);

    // optional data
    bool addMovement(int airport_id, bool is_landing, bool is_night, bool is_autoland = false);
    void addRemarks(const QString &remarks);
    bool addSecondPilot(int pilot_id);
    // bool addApproach(const QString &approach_type);

    void setFlightId(int flight_id) { m_flight_id = flight_id; };
    int flightId() const { return m_flight_id; }
    void setEventId(int event_id) { m_event_id = event_id; }
    int eventId() const { return m_event_id; }

    LogEntry logEntry() const;
    FlightLogEntry flightLogEntry() const;
    QList<FlightSegmentEntry> flightSegments() const;
    QList<MovementEntry> movements() const;
    QList<ApproachEntry> approaches() const;

  private:
    QStringList m_errors = {};

    // Intialise mandatory entries as invalid
    int m_date_jd        = -1;
    int m_departure_id   = -1;
    int m_destination_id = -1;
    int m_time_off_ms    = -1;
    int m_time_on_ms     = -1;
    int m_pic_id         = -1;
    int m_tail_id        = -1;
    int m_event_id       = -1;
    int m_flight_id      = -1;

    // optional entries
    std::optional<int> m_second_pilot_id;
    std::optional<int> m_third_pilot_id;
    std::optional<int> m_fourth_pilot_id;
    std::optional<QString> m_remarks;
    std::optional<QString> m_flight_number;

    struct MovementData {
        int airport_id;
        bool isLanding;
        bool isNight;
        bool isAutoland;
    };
    struct ApproachData {
        int airport_id;
        QString approach_type;
    };

    QList<FlightSegmentBuilder::SegmentData> m_segment_data;
    QList<MovementData> m_movement_event_data;
    QList<ApproachData> m_approach_data;
    //
    const static inline QString EVENT_TYPE = QStringLiteral("FLT");
};

} // namespace OPL
#endif // FLIGHTDATABUILDER_H
