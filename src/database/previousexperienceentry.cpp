/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#include "previousexperienceentry.h"

namespace OPL {

PreviousExperienceEntry::PreviousExperienceEntry()
    : Row(DbTable::PreviousExperience, 0)
{}

PreviousExperienceEntry::PreviousExperienceEntry(const RowData_T &row_data)
    : Row(DbTable::PreviousExperience, 0, row_data)
{}

PreviousExperienceEntry::PreviousExperienceEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::PreviousExperience, row_id, row_data)
{}

const QString PreviousExperienceEntry::getTableName() const
{
    return TABLE_NAME;
}

} // namespace OPL
