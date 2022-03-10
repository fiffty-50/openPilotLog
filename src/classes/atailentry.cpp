/*
 *openPilotLog - A FOSS Tail Logbook Application
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
#include "atailentry.h"
#include "src/opl.h"

ATailEntry::ATailEntry()
    : AEntry::AEntry(DataPosition(OPL::Db::TABLE_TAILS, 0))
{}

ATailEntry::ATailEntry(RowId_T row_id)
    : AEntry::AEntry(DataPosition(OPL::Db::TABLE_TAILS, row_id))
{}

ATailEntry::ATailEntry(RowData_T table_data)
    : AEntry::AEntry(DataPosition(OPL::Db::TABLE_TAILS, 0), table_data)
{}

const QString ATailEntry::registration()
{
    return getData().value(OPL::Db::TAILS_REGISTRATION).toString();
}

const QString ATailEntry::type()
{
    QString type_string;
    if (!tableData.value(OPL::Db::TAILS_MAKE).toString().isEmpty())
        type_string.append(getData().value(OPL::Db::TAILS_MAKE).toString() + ' ');
    if (!tableData.value(OPL::Db::TAILS_MODEL).toString().isEmpty())
        type_string.append(getData().value(OPL::Db::TAILS_MODEL).toString());
    if (!tableData.value(OPL::Db::TAILS_VARIANT).toString().isEmpty())
        type_string.append('-' + getData().value(OPL::Db::TAILS_VARIANT).toString());

    return type_string;
}
