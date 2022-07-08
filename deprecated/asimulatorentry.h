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
#ifndef ASIMULATORENTRY_H
#define ASIMULATORENTRY_H

#include "aentry.h"
#include "src/opl.h"

class ASimulatorEntry : public AEntry
{
public:
    ASimulatorEntry()
        : AEntry::AEntry(DataPosition(OPL::Db::TABLE_SIMULATORS, 0)){};
    ASimulatorEntry(RowId_T row_id)
        : AEntry::AEntry(DataPosition(OPL::Db::TABLE_SIMULATORS, row_id)){};
    ASimulatorEntry(RowData_T table_data)
        : AEntry::AEntry(DataPosition(OPL::Db::TABLE_SIMULATORS, 0), table_data){};

    ASimulatorEntry(const ASimulatorEntry& pe) = default;
    ASimulatorEntry& operator=(const ASimulatorEntry& pe) = default;
};

#endif // ASIMULATORENTRY_H
