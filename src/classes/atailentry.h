/*
 *openPilotLog - A FOSS Tail Logbook Application
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
#ifndef ATAILENTRY_H
#define ATAILENTRY_H

#include "src/classes/aentry.h"
#include "src/database/adatabasetypes.h"

struct ATailEntry : public AEntry {
public:
    ATailEntry();
    ATailEntry(const ATailEntry& te) = default;
    ATailEntry& operator=(const ATailEntry& te) = default;
    ATailEntry(RowId_T row_id);
    ATailEntry(RowData_T table_data);

    const QString registration();

    const QString type();
};


#endif // ATAILENTRY_H
