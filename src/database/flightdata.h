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
#ifndef FLIGHTDATA_H
#define FLIGHTDATA_H

#include "src/database/flightlogentry.h"
#include "src/database/flightsegmententry.h"
#include "src/database/logentry.h"
#include "src/database/movemententry.h"

namespace OPL {

/*!
 * \brief The FlightData class encapsulates a Flight retreived from the database
 * \details This class is a helper class to collect all data related to a single
 * flight from the seperate database tables. It is designed to assume that data
 * coming from the database is valid and as such only performs some basic validation.
 *
 * Most of the validation for logic and rules is handled when entering data into the
 * database, see also the FlightDataBuilder class.
 *
 * This class collects information and has some convenience functions to summarize data
 * for example the getTakeOffCount() and getLandingCount() functions summarize all data
 * from the movement table while the getBlockTime() summarizes all data from the flight_segments
 * table. The booleans for flight_segment data only refer to the value stored in the first
 * flight segment. For a more granular view of the data, the movementEntries() and flightSegments()
 * getters provide access to the underlying data.
 */
class FlightData {
  public:
    FlightData(const LogEntry &log_entry, const FlightLogEntry &flight_entry,
               const QList<FlightSegmentEntry> &segments, const QList<MovementEntry> &movements);
    // FlightData(const LogEntry &log_entry, const FlightLogEntry &flight_entry, const
    // QList<MovementEntry> &movements, const QList<Approachentry> &approaches);

    const LogEntry *logEntry() const { return &m_log_entry; }
    const FlightLogEntry *flightEntry() const { return &m_flight_entry; }
    const QList<MovementEntry> *movementEntries() const { return &m_movements; }
    const QList<FlightSegmentEntry> *flightSegments() const { return &m_segments; }

    /*!
     * \brief return the total amount of take offs performed on this flight
     */
    int getTakeOffCount() const;

    /*!
     * \brief return the total amount of landings performed on this flight
     */
    int getLandingCount() const;

    /*!
     * \brief returns the total block time of the flight
     */
    QTime getBlockTime() const;

    /*!
     * \brief returns the total night time of the flight
     */
    QTime getNightTime() const;

    /*!
     * \brief returns the pilot function of the first flight segment
     */
    QString pilotFunction() const;

    /*!
     * \brief retuns whether the pilot was pilot flying on the first segment of the flight
     */
    bool isPilotFlying() const;

    /*!
     * \brief returns whether the first flight segment is IFR
     */
    bool isIfr() const;

    /*!
     * \brief returns whether the first flight segment is simulated IFR
     */
    bool isSimulatedIfr() const;

    /*!
     * \brief returns whether the first flight segment is multi pilot time
     */
    bool isMultiPilot() const;

  private:
    const LogEntry m_log_entry;
    const FlightLogEntry m_flight_entry;
    const QList<FlightSegmentEntry> m_segments;
    const QList<MovementEntry> m_movements;
    // const QList<ApproachEntry> m_approach_entries;
};

} // namespace OPL

#endif // FLIGHTDATA_H
