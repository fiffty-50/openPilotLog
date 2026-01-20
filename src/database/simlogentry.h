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

#ifndef SIMLOGENTRY_H
#define SIMLOGENTRY_H

#include "src/database/row.h"
#include "src/opl.h"
#include <qobject.h>

namespace OPL {

class SimLogEntry : public Row {
  public:
    explicit SimLogEntry();
    explicit SimLogEntry(int row_id, const RowData_T &row_data);

    bool isValid() const override;

    bool setEventId(int event_id);
    bool setSimType(const QString &sim_type);
    bool setAircraftTypeId(int aircraft_type_id);
    bool setInstructorId(int instructor_id);
    bool setSecondPilotId(int second_pilot_id);
    bool setDurationMs(int duration_ms);

    int getEventId() const;
    QString getSimType() const;
    int getAircraftTypeId() const;
    int getInstructorId() const;
    int getSecondPilotId() const;
    int getDurationMs() const;

  private:
    const static inline QString EVENT_ID         = QStringLiteral("event_id");
    const static inline QString SIM_TYPE         = QStringLiteral("sim_type");
    const static inline QString AIRCRAFT_TYPE_ID = QStringLiteral("aircraft_type_id");
    const static inline QString INSTRUCTOR_ID    = QStringLiteral("instructor_id");
    const static inline QString SECOND_PILOT_ID  = QStringLiteral("second_pilot_id");
    const static inline QString DURATION         = QStringLiteral("duration_ms");

    const static inline QList<QString> FIELDS = {
        EVENT_ID, SIM_TYPE, AIRCRAFT_TYPE_ID, INSTRUCTOR_ID, SECOND_PILOT_ID, DURATION,
    };
};

} // namespace OPL

#endif // SIMLOGENTRY_H
