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
#include "src/opl.h"
#include "src/classes/date.h"
#include "src/classes/time.h"

namespace OPL {

FlightEntry::FlightEntry()
    : Row(DbTable::Flights, NEW_ENTRY)
{
    for(const QString &item : allFields) {
        rowData.insert(item, QVariant(QMetaType(QMetaType::Int)));
    }
    for(const QString &item : mandatoryFields) {
        rowData.insert(item, -1);
    }
}


FlightEntry::FlightEntry(const RowData_T &row_data)
    : Row(DbTable::Flights, NEW_ENTRY, row_data)
{
    for(const QString &item : allFields) {
        rowData.insert(item, QVariant(QMetaType(QMetaType::Int)));
    }
    for(const QString &item : mandatoryFields) {
        rowData.insert(item, -1);
    }

}

FlightEntry::FlightEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Flights, row_id, row_data) {}

const QString FlightEntry::getTableName() const
{
    return TABLE_NAME;
}

QString FlightEntry::getFlightSummary() const
{
    using namespace OPL;
    if(!isValid())
        return QString();

    auto tableData = getData();
    QString flight_summary;
    constexpr auto space = QLatin1Char(' ');
    flight_summary.append(OPL::Date(tableData.value(FlightEntry::DOFT).toInt(), DateTimeFormat()).toString() + space);
    flight_summary.append(tableData.value(FlightEntry::DEPT).toString() + space);
    flight_summary.append(Time(tableData.value(FlightEntry::TOFB).toInt(), DateTimeFormat()).toString()
                          + space);
    flight_summary.append(Time(tableData.value(FlightEntry::TONB).toInt(), DateTimeFormat()).toString()
                          + space);
    flight_summary.append(tableData.value(FlightEntry::DEST).toString());

    return flight_summary;
}
} // namespace OPL
