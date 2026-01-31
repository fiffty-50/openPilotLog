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

#include "src/database/flightlogentry.h"
#include "src/database/logentry.h"
#include "src/gui/verification/validationstate.h"
#include <optional>
#include <src/opl.h>

namespace OPL {

class FlightDataBuilder {
  public:
    FlightDataBuilder() = default;
    bool isValid() const;

    // mandatory data
    bool addDate(int date_jd);
    bool addDepartureLocation(int departure_id);
    bool addDestinationLocation(int destination_id);
    bool addTimeOffBlocks(int time_ms);
    bool addTimeOnBlocks(int time_ms);
    bool addPic(int pilot_id);
    bool addTail(int tail_id);

    // optional data
    bool addMovement(int airport_id, bool is_landing, bool is_night, bool is_autoland);
    void addRemarks(const QString &remarks);
    // bool addApproach(const QString &approach_type);

    std::optional<LogEntry> createLogEntry();
    std::optional<FlightLogEntry> createFlightLogEntry();

  private:
    ValidationState m_validationState;

    // Intialise mandatory entries as invalid
    int m_event_id       = -1;
    int m_date_jd        = -1;
    int m_departure_id   = -1;
    int m_destination_id = -1;
    int m_time_off_ms    = -1;
    int m_time_on_ms     = -1;
    int m_pic_id         = -1;
    int m_tail_id        = -1;

    // optional entries
    QString m_remarks       = QString();
    QString m_flight_number = QString();
    int m_second_pilot_id   = -1;
    int m_third_pilot_id    = -1;
    int m_fourth_pilot_id   = -1;

    struct MovementEventData {
        int airport_id;
        bool isLanding;
        bool isNight;
        bool isAutoland;
    };

    QList<MovementEventData> m_movement_event_data;
    RowData_T m_flight_data;
    RowData_T m_log_entry_data;
    // QList<ApproachEvent> m_approaches;
    //
    const static inline QString EVENT_TYPE = QStringLiteral("FLT");
};

} // namespace OPL
#endif // FLIGHTDATABUILDER_H
