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
    : table(table_name), rowId(row_id)
{
    hasData = false;
}

Row::Row(DbTable table_name)
    : table(table_name)
{
    hasData = false;
    rowId = 0; // new entry
};

Row::Row()
{
    valid = false;
}

Row::Row(OPL::DbTable table_name, int row_id, const RowData_T &row_data)
    : table(table_name), rowId(row_id), rowData(row_data)
{
    hasData = true;
};

const RowData_T &Row::getData() const
{
    return rowData;
}

void Row::setData(const RowData_T &value)
{
    rowData = value;
    hasData = true;
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
    return hasData && valid;
}

//Used for debugging
OPL::Row::operator QString() const
{
    if (!isValid()) {
        return QStringLiteral("Invalid Row");
    }
    QString out("\033[32m[Entry Data]:\t\033[m\n");
    int item_count = 0;
    QHash<QString, QVariant>::const_iterator i;
    for (i = rowData.constBegin(); i!= rowData.constEnd(); ++i) {
        QString spacer(":");
        int spaces = (14 - i.key().length());
        if (spaces > 0)
            for (int i = 0; i < spaces ; i++)
                spacer += QLatin1Char(' ');
        if (i.value().toString().isEmpty()) {
            out.append(QLatin1String("\t\033[m") + i.key()
                       + spacer
                       + QLatin1String("\033[35m----"));
            spaces = (14 - i.value().toString().length());
            spacer = QString();
            if (spaces > 0)
                for (int i = 0; i < spaces ; i++)
                    spacer += QLatin1Char(' ');
            out.append(spacer);
        } else {
            out.append(QLatin1String("\t\033[m") + i.key()
                       + spacer
                       + QLatin1String("\033[35m")
                       + i.value().toString());

            spaces = (14 - i.value().toString().length());
            spacer = QString();
            if (spaces > 0)
                for (int i = 0; i < spaces ; i++)
                    spacer += QLatin1Char(' ');
            out.append(spacer);
        }
        item_count ++;
        if (item_count % 4 == 0)
            out.append(QLatin1String("\n"));
    }
    out.append(QLatin1String("\n"));
    QTextStream(stdout) << out;
    return QString();
}

} // namespace OPL

