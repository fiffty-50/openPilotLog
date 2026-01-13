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
#include "aircraftentry.h"
#include <QSqlQuery>

namespace OPL {

AircraftEntry::AircraftEntry()
    : Row(DbTable::Aircraft, 0)
{}

AircraftEntry::AircraftEntry(const RowData_T &row_data)
    : Row(DbTable::Aircraft, 0, row_data)
{}

AircraftEntry::AircraftEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Aircraft, row_id, row_data)
{}

const QString AircraftEntry::getTableName() const
{
    return TABLE_NAME;
}

QString AircraftEntry::getTypeString(int aircraft_id)
{
    const QString statement = QStringLiteral("SELECT make||' '||model AS ident "
                                        "FROM aircraft_types "
                                        "WHERE model IS NOT NULL "
                                        "AND variant IS NULL "
                                        "AND aircraft_type_id = ? "
                                        "UNION "
                                        "SELECT make||' '||model||'-'||variant AS ident "
                                        "FROM aircraft_types "
                                        "WHERE variant IS NOT NULL "
                                        "AND aircraft_type_id = ?");
    QSqlQuery query;
    query.prepare(statement);
    query.setForwardOnly(true);
    query.exec();

    if(!query.next()) {
        return QStringLiteral("Aircraft type not in database");
    } else {
        return query.value(0).toString();
    }
}

} // namespace OPL
