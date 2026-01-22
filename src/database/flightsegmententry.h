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
#ifndef FLIGHTSEGMENTENTRY_H
#define FLIGHTSEGMENTENTRY_H

#include "row.h"

namespace OPL {

// Only checks for semantically correct data, this is mainly the database interface
// used for retreiving data from the database which is assumed to be correct.
// logic checks are carried out in the builder class which is why the default constructor
// is deleted here
class FlightSegmentEntry : public OPL::Row {
  public:
    FlightSegmentEntry() = delete;
    FlightSegmentEntry(int event_id, const OPL::RowData_T &row_data);

    bool isValid() const override;

    int startMs() const { return m_rowData.value(START_MS).toInt(); }
    int endMs() const { return m_rowData.value(END_MS).toInt(); }
    bool isIfr() const { return m_rowData.value(IS_IFR).toBool(); }
    bool isSimulatedIfr() const { return m_rowData.value(IS_SIM_IFR).toBool(); }
    bool isNight() const { return m_rowData.value(IS_NIGHT).toBool(); }
    bool isMultiPilot() const { return m_rowData.value(IS_MULTI_PILOT).toBool(); }
    bool isPilotFlying() const { return m_rowData.value(IS_PILOT_FLYING).toBool(); }
    QString pilotFunction() const { return m_rowData.value(PILOT_FUNCTION).toString(); }

  private:
    const static inline QString TABLE_NAME      = QStringLiteral("flight_segments");
    const static inline QString FLIGHT_ID       = QStringLiteral("flight_id");
    const static inline QString START_MS        = QStringLiteral("start_ms");
    const static inline QString END_MS          = QStringLiteral("end_ms");
    const static inline QString IS_IFR          = QStringLiteral("is_ifr");
    const static inline QString IS_SIM_IFR      = QStringLiteral("is_simulated_ifr");
    const static inline QString IS_NIGHT        = QStringLiteral("is_night");
    const static inline QString IS_MULTI_PILOT  = QStringLiteral("is_multi_pilot");
    const static inline QString IS_PILOT_FLYING = QStringLiteral("is_pilot_flying");
    const static inline QString PILOT_FUNCTION  = QStringLiteral("pilot_function");

    const static inline QStringList FIELDS = {FLIGHT_ID,      START_MS,        END_MS,
                                              IS_IFR,         IS_SIM_IFR,      IS_NIGHT,
                                              IS_MULTI_PILOT, IS_PILOT_FLYING, PILOT_FUNCTION};
};

} // namespace OPL
#endif // FLIGHTSEGMENTENTRY_H
