/*
 *openPilotLog - A FOSS Pilot Logbook Application
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
#include "aentry.h"

AEntry::AEntry(DataPosition position_)
    : position(position_)
{}

AEntry::AEntry(RowData_T table_data)
    : tableData(table_data)
{}

AEntry::AEntry(DataPosition position_, RowData_T table_data)
    : position(position_), tableData(table_data)
{}

void AEntry::setData(RowData_T table_data)
{
    tableData = table_data;
}

void AEntry::setPosition(DataPosition position_)
{
    position = position_;
}

const DataPosition& AEntry::getPosition() const
{
    return position;
}

const RowData_T& AEntry::getData() const
{
    return tableData;
}

AEntry::operator QString() const
{
    QString out("\033[32m[Entry Data]:\t\033[m\n");
    int item_count = 0;
    QMap<ColName_T, ColData_T>::const_iterator i;
    for (i = tableData.constBegin(); i!= tableData.constEnd(); ++i) {
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
