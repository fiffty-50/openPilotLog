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
#include "flightdatabuilder.h"

namespace OPL {

FlightDataBuilder::FlightDataBuilder() {}

bool FlightDataBuilder::addMovement(int airport_id, bool is_landing, bool is_night, bool is_autoland)
{
    if(airport_id < 1) {
        DEB << "No airport_id provided for movement.";
    }
    if(airport_id < 0) return false;

    m_movement_event_data.append({airport_id, is_landing, is_night, is_autoland });
    return true;
}
} // namespace OPL
