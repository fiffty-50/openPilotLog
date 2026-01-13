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
#include "tailentry.h"
#include "src/database/databasecache.h"

namespace OPL {

TailEntry::TailEntry()
    : Row(DbTable::Tails, 0)
{
    clear();
}

TailEntry::TailEntry(const RowData_T &row_data)
    : Row(DbTable::Tails, 0, row_data)
{}

TailEntry::TailEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Tails, row_id, row_data)
{}

void TailEntry::clear()
{
    for (const auto &field : fields) {
        rowData.insert(field, QVariant(QMetaType(QMetaType::QString)));
    }
}

const QString TailEntry::getTableName() const
{
    return TABLE_NAME;
}

bool TailEntry::isValid() const
{
    bool valid = true;

    // a valid type ID must be set
    valid &= (DBCache->getMap(DatabaseCache::MapType::AircraftTypes).value(rowData.value(TYPE_ID).toInt()) != QString());

    // registration must not be empty
    valid &= (rowData.value(REGISTRATION).toString() != QString());

    // in-service date must be a valid julian date
    const int jd = rowData.value(IN_SERVICE_DATE).toInt();
    valid &= (jd != 0);
    valid &= QDate::fromJulianDay(rowData.value(IN_SERVICE_DATE).toInt()).isValid();

    // the other fields are optional
    return valid;
}

bool TailEntry::setRegistration(const QString &registration)
{
    if (registration.isEmpty())
        return false;

    rowData.insert(REGISTRATION, registration);
    return true;
}

bool TailEntry::setInServiceDate(const QDate &date)
{
    if(!date.isValid())
        return false;

    rowData.insert(IN_SERVICE_DATE, date.toJulianDay());
    return true;
}

bool TailEntry::setOutOfServiceDate(const QDate &date)
{
    if(!date.isValid()) {
        DEB << "Invalid Date.";
        return false;
    }

    rowData.insert(OUT_OF_SERVICE_DATE, date.toJulianDay());
    return true;
}

bool TailEntry::setTypeId(int typeId)
{
    bool isValid = typeId != 0;

    isValid &= DBCache->getMap(DatabaseCache::MapType::AircraftTypes)
                   .value(typeId) != QString();

    if(!isValid)
        return false;

    rowData.insert(TYPE_ID, typeId);
    return true;
}


} // namespace OPL
