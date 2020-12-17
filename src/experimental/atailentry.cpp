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

namespace experimental {

ATailEntry::ATailEntry()
    : AEntry::AEntry(DEFAULT_TAIL_POSITION)
{}

ATailEntry::ATailEntry(int row_id)
    : AEntry::AEntry(DataPosition("tails", row_id))
{}

ATailEntry::ATailEntry(TableData table_data)
    : AEntry::AEntry(DEFAULT_TAIL_POSITION, table_data)
{}

const QString ATailEntry::registration()
{
    return getData().value("registration");
}

const QString ATailEntry::type()
{
    QString type_string;
    if (!getData().value("make").isEmpty())
        type_string.append(getData().value("make") + ' ');
    if (!getData().value("model").isEmpty())
        type_string.append(getData().value("model"));
    if (!getData().value("variant").isEmpty())
        type_string.append('-' + getData().value("variant") + ' ');

    return type_string;
}

} // namespace experimental