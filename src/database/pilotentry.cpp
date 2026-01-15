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
#include "pilotentry.h"

namespace OPL {

PilotEntry::PilotEntry()
    : Row(DbTable::Pilots, FIELDS)
{}

PilotEntry::PilotEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Pilots, row_id, row_data, FIELDS)
{}

bool PilotEntry::isValid() const
{
    return ! m_rowData.value(NAME).toString().isEmpty();
}

bool PilotEntry::setName(const QString &input)
{
    if(input.isEmpty())
        return false;

    m_rowData.insert(NAME, input);
    return true;
}

void PilotEntry::setAlias(const QString &input)
{
    m_rowData.insert(ALIAS, input);
}

void PilotEntry::setEmployeeId(const QString &input)
{
    m_rowData.insert(EMPLOYEEID, input);
}

void PilotEntry::setCompany(const QString &input)
{
    m_rowData.insert(COMPANY, input);
}

void PilotEntry::setPhone(const QString &input)
{
    m_rowData.insert(PHONE, input);
}

void PilotEntry::setEmail(const QString &input)
{
    m_rowData.insert(EMAIL, input);
}

void PilotEntry::setRemarks(const QString &input)
{
    m_rowData.insert(REMARKS, input);
}

const QString PilotEntry::getName() const
{
    return m_rowData.value(NAME).toString();
}

const QString PilotEntry::getAlias() const
{
    return m_rowData.value(ALIAS).toString();
}

const QString PilotEntry::getEmployeeId() const
{
    return m_rowData.value(EMPLOYEEID).toString();
}

const QString PilotEntry::getCompany() const
{
    return m_rowData.value(COMPANY).toString();
}

const QString PilotEntry::getPhone() const
{
    return m_rowData.value(PHONE).toString();
}

const QString PilotEntry::getEmail() const
{
    return m_rowData.value(EMAIL).toString();
}

const QString PilotEntry::getRemarks() const
{
    return m_rowData.value(REMARKS).toString();
}

} // namespace OPL
