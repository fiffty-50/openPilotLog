/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#ifndef AAIRCRAFTENTRY_H
#define AAIRCRAFTENTRY_H

#include "src/classes/aentry.h"
#include "src/database/adatabasetypes.h"

struct AAircraftEntry : public AEntry {
public:
    AAircraftEntry();
    AAircraftEntry(RowId_T row_id);
    AAircraftEntry(RowData_T table_data);

    AAircraftEntry(const AAircraftEntry& te) = default;
    AAircraftEntry& operator=(const AAircraftEntry& te) = default;
};

#endif // AAIRCRAFTENTRY_H
