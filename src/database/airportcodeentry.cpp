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
#include "airportcodeentry.h"

namespace OPL {

AirportCodeEntry::AirportCodeEntry()
    : Row(DbTable::v2AirportCodes, 0){}

AirportCodeEntry::AirportCodeEntry(const RowData_T &rowData)
    : Row(DbTable::v2AirportCodes, 0, rowData){}

AirportCodeEntry::AirportCodeEntry(int rowId, const RowData_T &rowData)
    : Row(DbTable::v2AirportCodes, rowId, rowData){}

const QString AirportCodeEntry::getIataCode() const
{
    Q_UNIMPLEMENTED();
    return QString();
}

const QString AirportCodeEntry::getIcaoCode() const
{
    Q_UNIMPLEMENTED();
    return QString();
}

const QString AirportCodeEntry::getCustomCode() const
{
    Q_UNIMPLEMENTED();
    return QString();
}

bool AirportCodeEntry::setAirportCode(CodeType type, const QDate &validFrom, const QDate &validTo)
{
    Q_UNIMPLEMENTED();
    return false;
}


} // namespace OPL
