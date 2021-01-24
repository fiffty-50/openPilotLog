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
#include "aentry.h"

AEntry::AEntry(DataPosition position_)
    : position(position_)
{}

AEntry::AEntry(RowData_t table_data)
    : tableData(table_data)
{}

AEntry::AEntry(DataPosition position_, RowData_t table_data)
    : position(position_), tableData(table_data)
{}

void AEntry::setData(RowData_t table_data)
{
    tableData = table_data;
}

const DataPosition& AEntry::getPosition()
{
    return position;
}

const RowData_t& AEntry::getData()
{
    return tableData;
}
