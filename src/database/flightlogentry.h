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

#ifndef FLIGHTLOGENTRY_H
#define FLIGHTLOGENTRY_H

#include "logentry.h"
#include "row.h"
#include "src/opl.h"
#include <qobject.h>

namespace OPL {

class FlightLogEntry : public Row {
  public:
    explicit FlightLogEntry();
    explicit FlightLogEntry(int rowId, const RowData_T &rowData);

    bool isValid() const override;

    bool setLogEntry(const LogEntry &logEntry);
    const LogEntry *getLogEntry() const;

    bool setEventId(int event_id);
    bool setDeparture(int airport_id);
    bool setDestination(int airport_id);
    bool setTimeOffBlocks(int time_ms);
    bool setTimeOnBlocks(int time_ms);
    bool setPic(int pilot_id);
    bool setSecondPilot(int pilot_id);
    bool setThirdPilot(int pilot_id);
    bool setFourthPilot(int pilot_id);
    bool setTail(int tail_id);
    bool setFlightNumber(const QString &flight_number);

    int getEventId() const;
    int getDepartureId() const;
    int getDestinationId() const;
    int getTimeOffBlocksMs() const;
    int getTimeOnBlocksMs() const;
    int getPicId() const;
    int getSecondPilotId() const;
    int getThirdPilotId() const;
    int getFourthPilotId() const;
    int getTailId() const;
    QString getFlightNumber() const;

  private:
    const static inline QString EVENT_ID      = QStringLiteral("event_id");
    const static inline QString DEPARTURE     = QStringLiteral("departure");
    const static inline QString DESTINATION   = QStringLiteral("destination");
    const static inline QString T_OFF_BLOCK   = QStringLiteral("tofb_ms");
    const static inline QString T_ON_BLOCK    = QStringLiteral("tonb_ms");
    const static inline QString PILOT_PIC     = QStringLiteral("pic_pilot_id");
    const static inline QString PILOT_TWO     = QStringLiteral("second_pilot_id");
    const static inline QString PILOT_THREE   = QStringLiteral("third_pilot_id");
    const static inline QString PILOT_FOUR    = QStringLiteral("fourth_pilot_id");
    const static inline QString TAIL          = QStringLiteral("tail_id");
    const static inline QString FLIGHT_NUMBER = QStringLiteral("flight_number");

    const static inline QList<QString> FIELDS = {
        EVENT_ID,  DEPARTURE,   DESTINATION, T_OFF_BLOCK, T_ON_BLOCK,    PILOT_PIC,
        PILOT_TWO, PILOT_THREE, PILOT_FOUR,  TAIL,        FLIGHT_NUMBER,
    };

    LogEntry m_logEntry;
};

} // namespace OPL

#endif // FLIGHTLOGENTRY_H
