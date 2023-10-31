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
#include "flightentry.h"
#include "src/classes/date.h"
#include "src/classes/time.h"

namespace OPL {

FlightEntry::FlightEntry()
    : Row(DbTable::Flights, 0)
{}

FlightEntry::FlightEntry(const RowData_T &row_data)
    : Row(DbTable::Flights, 0, row_data)
{}

FlightEntry::FlightEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Flights, row_id, row_data)
{}

const QString FlightEntry::getTableName() const
{
    return TABLE_NAME;
}

const QString FlightEntry::getFlightSummary() const
{
    if(!isValid())
        return QString();

    auto tableData = getData();
    QString flight_summary;
    auto space = QLatin1Char(' ');
    flight_summary.append(OPL::Date(tableData.value(OPL::FlightEntry::DOFT).toInt()).toString() + space);
    flight_summary.append(tableData.value(OPL::FlightEntry::DEPT).toString() + space);
    flight_summary.append(OPL::Time(tableData.value(OPL::FlightEntry::TOFB).toInt()).toString()
                          + space);
    flight_summary.append(OPL::Time(tableData.value(OPL::FlightEntry::TONB).toInt()).toString()
                          + space);
    flight_summary.append(tableData.value(OPL::FlightEntry::DEST).toString());

    return flight_summary;
}



} // namespace OPL
