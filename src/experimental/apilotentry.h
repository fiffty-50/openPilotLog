/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
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

#include "src/experimental/aentry.h"
#include "src/experimental/decl.h"

namespace experimental {

struct APilotEntry : public AEntry {
public:
    APilotEntry();
    APilotEntry(const APilotEntry& pe) = default;
    APilotEntry& operator=(const APilotEntry& pe) = default;
    APilotEntry(int row_id);
    APilotEntry(TableData table_data);
};

} // namespace experimental

#endif // APILOTENTRY_H
