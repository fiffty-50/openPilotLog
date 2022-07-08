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
#include "aflightentry.h"
#include "src/database/adatabase.h"
#include "src/opl.h"
#include "src/functions/atime.h"
#include "src/classes/asettings.h"

AFlightEntry::AFlightEntry()
    : AEntry::AEntry(DataPosition(OPL::Db::TABLE_FLIGHTS, 0))
{}

AFlightEntry::AFlightEntry(RowId_T row_id)
    : AEntry::AEntry(DataPosition(OPL::Db::TABLE_FLIGHTS, row_id))
{}

AFlightEntry::AFlightEntry(RowData_T table_data)
    : AEntry::AEntry(DataPosition(OPL::Db::TABLE_FLIGHTS, 0), table_data)
{}

const QString AFlightEntry::summary()
{
    if(tableData.isEmpty())
        return QString();

    QString flight_summary;
    auto space = QLatin1Char(' ');
    flight_summary.append(tableData.value(OPL::Db::FLIGHTS_DOFT).toString() + space);
    flight_summary.append(tableData.value(OPL::Db::FLIGHTS_DEPT).toString() + space);
    flight_summary.append(ATime::toString(tableData.value(OPL::Db::FLIGHTS_TOFB).toInt())
                          + space);
    flight_summary.append(ATime::toString(tableData.value(OPL::Db::FLIGHTS_TONB).toInt())
                          + space);
    flight_summary.append(tableData.value(OPL::Db::FLIGHTS_DEST).toString());

    return flight_summary;
}
