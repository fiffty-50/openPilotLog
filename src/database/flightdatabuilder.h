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

#include <array>
#include <src/opl.h>

namespace OPL {

class FlightDataBuilder {
  public:
    FlightDataBuilder();

  private:
    using Movement = std::array<int, 4>;

    int m_event_id;
    QList<Movement> m_movement_data;
    RowData_T m_flight_data;
    RowData_T m_log_entry_data;
    // QList<ApproachEvent> m_approaches;
};

} // namespace OPL
#endif // FLIGHTDATABUILDER_H
