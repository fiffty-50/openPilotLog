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
#include "row.h"

namespace OPL {

Row::Row(OPL::DbTable table_name, int row_id)
    : table(table_name), rowId(row_id) {}

Row::Row(DbTable table_name)
    : table(table_name)
{
    rowId = 0; // new entry
};

Row::Row(OPL::DbTable table_name, int row_id, const RowData_T &row_data)
    : table(table_name), rowId(row_id), rowData(row_data) {}

const RowData_T &Row::getData() const
{
    return rowData;
}

void Row::setData(const RowData_T &value)
{
    rowData = value;
}

int Row::getRowId() const
{
    return rowId;
}

void Row::setRowId(int value)
{
    rowId = value;
}

OPL::DbTable Row::getTable() const
{
    return table;
}

const QString Row::getPosition() const
{
    return QString("Table: %1 / RowID: %2").arg(OPL::GLOBALS->getDbTableName(table), QString::number(rowId));
}

const QString Row::getTableName() const
{
    return {};
}

bool Row::isValid() const
{
    return !rowData.isEmpty();
}

//Used for debugging
OPL::Row::operator QString() const
{
    if (!isValid()) {
        return QStringLiteral("Invalid Row");
    }
    constexpr int ColumnWidth = 14;
    constexpr int ItemsPerRow = 4;

    const QString Reset  = "\033[m";
    const QString Green  = "\033[32m";
    const QString Purple = "\033[35m";

    QString out;
    out.reserve(1024);

    out += Green + "[Entry Data]:\t" + Reset + "\n";

    int itemCount = 0;

    for (auto it = rowData.cbegin(); it != rowData.cend(); ++it) {
        const QString key = it.key();
        const QString value = it.value().toString();

        QString paddedKey = key;
        paddedKey += QLatin1Char(':');
        paddedKey = paddedKey.leftJustified(ColumnWidth);

        const QString displayVal = value.isEmpty() ? "-NULL-" : value;
        const QString paddedValue = displayVal.leftJustified(ColumnWidth);

        out += "\t" % Reset
               % paddedKey
               % Purple
               % paddedValue;

        if (++itemCount % ItemsPerRow == 0)
            out += "\n";
    }

    out += "\n";
    QTextStream(stdout) << out;
    return QString();
}

} // namespace OPL

