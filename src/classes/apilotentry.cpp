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
#include "apilotentry.h"
#include "src/opl.h"

APilotEntry::APilotEntry()
    : AEntry::AEntry(Opl::Db::DEFAULT_PILOT_POSITION)
{}

APilotEntry::APilotEntry(RowId_T row_id)
    : AEntry::AEntry(DataPosition(Opl::Db::TABLE_PILOTS, row_id))
{}

APilotEntry::APilotEntry(RowData_T table_data)
    : AEntry::AEntry(Opl::Db::DEFAULT_PILOT_POSITION, table_data)
{}

const QString APilotEntry::name()
{
    if (tableData.isEmpty())
        return QString();

    return tableData.value(Opl::Db::PILOTS_LASTNAME).toString() + ", "
           //+tableData.value(Opl::Db::PILOTS_FIRSTNAME).toString().left(1) + '.';
           +tableData.value(Opl::Db::PILOTS_FIRSTNAME).toString();
}
