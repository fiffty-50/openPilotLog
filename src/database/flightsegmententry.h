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

/*!
 * \brief The FlightSegmentEntry class represents a Row in the flight_segments table
 * \details A flight contains one ore more segments. If the conditions of the flight
 * are constant, a single segment can be used to describe an entire flight. Some flights
 * have conditions change during the flight however, for example Day/Night or transitions
 * from IFR/VFR.
 *
 * The following must be true for a flight:
 * <ul>
 * <li> Every flight must have one ore more segments </li>
 * <li> The sum of all segments bust be equal to the total block time </li>
 * <li> Segment start time is inclusive </li>
 * <li> Segment end time is exclusive </li>
 * <li> Within one segment, conditions do not change </li>
 * </ul>
 *
 * When a flight is updated, all segments are first deleted and then re-calculated from scratch.
 * This avoids complex logic for updating segments.
 *
 * \note The FlightSegmentEntry class only does basic checks for semantic validity of its data.
 * The check and calculation logic is done by the FlightSegmentBuilder class.
 *
 * \note Constructors for values based on the database take the segment_id (Primary Key) while
 * constructors for new segments take the associated flight_id (Foreign Key).
 */
class FlightSegmentEntry : public OPL::Row {
  public:
    FlightSegmentEntry() = delete;
    /*!
     * \brief Create a FlightSegmentEntry based on values from the database
     */
    FlightSegmentEntry(int segment_id, const OPL::RowData_T &row_data);

    /*!
     * \brief Create a new FlightSegmentEntry for an associated flight_id
     */
    FlightSegmentEntry(int flight_id, int start_ms, int end_ms, bool is_ifr, bool is_simulated_ifr,
                       bool is_night, bool is_multi_pilot, bool is_pilot_flying,
                       const QString &pilot_function);
    /*!
     * \brief Create a new FlightSegmentEntry for an associated flight_id
     */
    FlightSegmentEntry(int flight_id, int start_ms, int end_ms);

    bool isValid() const override;

    bool setFlightId(int flight_id);
    bool setStartMs(int start_ms);
    bool setEndMs(int end_ms);
    void setIsIfr(bool is_ifr);
    void setIsSimIfr(bool is_sim_ifr);
    void setIsNight(bool is_night);
    void setIsMultiPilot(bool is_multi_pilot);
    void setIsPilotFlying(bool is_pilot_flying);
    bool setPilotFunction(const QString &pilot_function);

    int getStartMs() const { return m_rowData.value(START_MS).toInt(); }
    int getEndMs() const { return m_rowData.value(END_MS).toInt(); }
    bool isIfr() const { return m_rowData.value(IS_IFR).toBool(); }
    bool isSimulatedIfr() const { return m_rowData.value(IS_SIM_IFR).toBool(); }
    bool isNight() const { return m_rowData.value(IS_NIGHT).toBool(); }
    bool isMultiPilot() const { return m_rowData.value(IS_MULTI_PILOT).toBool(); }
    bool isPilotFlying() const { return m_rowData.value(IS_PILOT_FLYING).toBool(); }
    QString getPilotFunction() const { return m_rowData.value(PILOT_FUNCTION).toString(); }

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
