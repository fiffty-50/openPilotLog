/*
 *openTail Log - A FOSS Tail Logbook Application
 *Copyright (C) 2020  Felix Turowsky
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
#include "src/oplconstants.h"

ATailEntry::ATailEntry()
    : AEntry::AEntry(DEFAULT_TAIL_POSITION)
{}

ATailEntry::ATailEntry(RowId row_id)
    : AEntry::AEntry(DataPosition(opl::db::TABLE_TAILS, row_id))
{}

ATailEntry::ATailEntry(RowData table_data)
    : AEntry::AEntry(DEFAULT_TAIL_POSITION, table_data)
{}

const QString ATailEntry::registration()
{
    return getData().value(opl::db::TAILS_REGISTRATION).toString();
}

const QString ATailEntry::type()
{
    QString type_string;
    if (!tableData.value(opl::db::TAILS_MAKE).toString().isEmpty())
        type_string.append(getData().value(opl::db::TAILS_MAKE).toString() + ' ');
    if (!tableData.value(opl::db::TAILS_MODEL).toString().isEmpty())
        type_string.append(getData().value(opl::db::TAILS_MODEL).toString());
    if (!tableData.value(opl::db::TAILS_VARIANT).toString().isEmpty())
        type_string.append('-' + getData().value(opl::db::TAILS_VARIANT).toString() + ' ');

    return type_string;
}
