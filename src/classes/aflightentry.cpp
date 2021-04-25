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
    : AEntry::AEntry(Opl::Db::DEFAULT_FLIGHT_POSITION)
{}

AFlightEntry::AFlightEntry(RowId_T row_id)
    : AEntry::AEntry(DataPosition(Opl::Db::TABLE_FLIGHTS, row_id))
{}

AFlightEntry::AFlightEntry(RowData_T table_data)
    : AEntry::AEntry(Opl::Db::DEFAULT_FLIGHT_POSITION, table_data)
{}

const QString AFlightEntry::summary()
{
    if(tableData.isEmpty())
        return QString();

    QString flight_summary;
    auto space = QStringLiteral(" ");
    flight_summary.append(tableData.value(Opl::Db::FLIGHTS_DOFT).toString() + space);
    flight_summary.append(tableData.value(Opl::Db::FLIGHTS_DEPT).toString() + space);
    flight_summary.append(ATime::toString(tableData.value(Opl::Db::FLIGHTS_TOFB).toInt())
                          + space);
    flight_summary.append(ATime::toString(tableData.value(Opl::Db::FLIGHTS_TONB).toInt())
                          + space);
    flight_summary.append(tableData.value(Opl::Db::FLIGHTS_DEST).toString());

    return flight_summary;
}

const QString AFlightEntry::getRegistration()
{
    ATailEntry acft = aDB->resolveForeignTail(tableData.value(Opl::Db::FLIGHTS_ACFT).toInt());
    return acft.registration();
}

const QString AFlightEntry::getPilotName(pilotPosition pilot_)
{
    switch (pilot_) {
    case pilotPosition::pic: {
        auto foreign_pilot = aDB->resolveForeignPilot(tableData.value(Opl::Db::FLIGHTS_PIC).toInt());
        return foreign_pilot.name();
        break;
    }
    case pilotPosition::secondPilot: {
        auto foreign_pilot = aDB->resolveForeignPilot(tableData.value(Opl::Db::FLIGHTS_SECONDPILOT).toInt());
        return foreign_pilot.name();
    }
    case pilotPosition::thirdPilot: {
        auto foreign_pilot = aDB->resolveForeignPilot(tableData.value(Opl::Db::FLIGHTS_THIRDPILOT).toInt());
        return foreign_pilot.name();
        break;
    } // case scope
    } // switch (pilot_)
    return QString();
}
