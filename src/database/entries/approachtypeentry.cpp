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
#include "approachtypeentry.h"
#include "src/opl.h"

namespace OPL {

ApproachTypeEntry::ApproachTypeEntry() : Row(OPL::DbTable::ApproachTypes, &FIELDS) {}

ApproachTypeEntry::ApproachTypeEntry(int row_id, const OPL::RowData_T &row_data)
    : Row(OPL::DbTable::ApproachTypes, row_id, row_data, &FIELDS)
{
}

bool ApproachTypeEntry::isValid() const { return !m_rowData.value(TYPE_NAME).toString().isEmpty(); }

bool ApproachTypeEntry::setName(const QString &name)
{
    if (name.isEmpty()) return false;

    m_rowData.insert(TYPE_NAME, name);
    return true;
}

} // namespace OPL
