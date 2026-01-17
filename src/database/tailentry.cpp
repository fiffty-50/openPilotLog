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
    : Row(DbTable::v2AircraftTails, &FIELDS)
{}

TailEntry::TailEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::v2AircraftTails, row_id, row_data, &FIELDS)
{}

bool TailEntry::isValid() const
{
    // a valid type ID must be set
    if(! m_rowData.contains(TYPE_ID)) {
        LOG << QStringLiteral("Type ID missing.");
        return false;
    }
    if(! (DBCache->getMap(DatabaseCache::MapType::AircraftTypes).value(m_rowData.value(TYPE_ID).toInt()) != QString()))  {
        LOG << QStringLiteral("Type ID invalid.");
        return false;   
    }
    
    // Registration must not be empty 
    if(! m_rowData.contains(REGISTRATION)) {
        LOG << QStringLiteral("Registration missing.");
        return false;
    }
    
    // In-service date must be valid
    if(! m_rowData.contains(IN_SERVICE_DATE)) {
        LOG << QStringLiteral("In-service date missing.");
        return false;
    }
    if(! QDate::fromJulianDay(m_rowData.value(IN_SERVICE_DATE).toInt()).isValid()) {
        LOG << QStringLiteral("In-service date invalid.");
        return false;
    }

    
    // The other fields are optional
    return true;
}

bool TailEntry::setRegistration(const QString &registration)
{
    if (registration.isEmpty())
        return false;

    m_rowData.insert(REGISTRATION, registration);
    return true;
}

bool TailEntry::setInServiceDate(const QDate &date)
{
    if(!date.isValid())
        return false;

    m_rowData.insert(IN_SERVICE_DATE, date.toJulianDay());
    return true;
}

bool TailEntry::setOutOfServiceDate(const QDate &date)
{
    if(!date.isValid()) {
        DEB << "Invalid Date.";
        return false;
    }

    m_rowData.insert(OUT_OF_SERVICE_DATE, date.toJulianDay());
    return true;
}

bool TailEntry::setTypeId(int typeId)
{
    bool isValid = typeId != 0;

    isValid &= DBCache->getMap(DatabaseCache::MapType::AircraftTypes)
                   .value(typeId) != QString();

    if(!isValid)
        return false;

    m_rowData.insert(TYPE_ID, typeId);
    return true;
}


} // namespace OPL
