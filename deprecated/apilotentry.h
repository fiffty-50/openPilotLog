/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
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
#ifndef APILOTENTRY_H
#define APILOTENTRY_H

#include "src/classes/aentry.h"
#include "src/database/databasetypes.h"

struct APilotEntry : public AEntry {
public:
    APilotEntry();
    APilotEntry(RowId_T row_id);
    APilotEntry(RowData_T table_data);

    APilotEntry(const APilotEntry& pe) = default;
    APilotEntry& operator=(const APilotEntry& pe) = default;

    const QString name();
};

#endif // APILOTENTRY_H
