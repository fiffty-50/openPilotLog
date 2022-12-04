/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
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

const QString Row::getTableName() const
{
    return OPL::GLOBALS->getDbTableName(table);
}

//TODO: Remove when tweaking for performance. Used for debugging
const QString Row::getPosition() const
{
    return QString("Table: %1 / RowID: %2").arg(OPL::GLOBALS->getDbTableName(table), QString::number(rowId));
}

//TODO: Remove when tweaking for performance. Used for debugging
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

AircraftEntry::AircraftEntry()
    : Row(DbTable::Aircraft, 0)
{}

AircraftEntry::AircraftEntry(const RowData_T &row_data)
    : Row(DbTable::Aircraft, 0, row_data)
{}

AircraftEntry::AircraftEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Aircraft, row_id, row_data)
{}


TailEntry::TailEntry()
    : Row(DbTable::Tails, 0)
{}

TailEntry::TailEntry(const RowData_T &row_data)
    : Row(DbTable::Tails, 0, row_data)
{}

TailEntry::TailEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Tails, row_id, row_data)
{}

PilotEntry::PilotEntry()
    : Row(DbTable::Pilots, 0)
{}

PilotEntry::PilotEntry(const RowData_T &row_data)
    : Row(DbTable::Pilots, 0, row_data)
{}

PilotEntry::PilotEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Pilots, row_id, row_data)
{}


SimulatorEntry::SimulatorEntry()
    : Row(DbTable::Simulators, 0)
{}

SimulatorEntry::SimulatorEntry(const RowData_T &row_data)
    : Row(DbTable::Simulators, 0, row_data)
{}

SimulatorEntry::SimulatorEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Simulators, row_id, row_data)
{}

FlightEntry::FlightEntry()
    : Row(DbTable::Flights, 0)
{}

FlightEntry::FlightEntry(const RowData_T &row_data)
    : Row(DbTable::Flights, 0, row_data)
{}

FlightEntry::FlightEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Flights, row_id, row_data)
{}

CurrencyEntry::CurrencyEntry()
    : Row(DbTable::Currencies, 0)
{}

CurrencyEntry::CurrencyEntry(const RowData_T &row_data)
    : Row(DbTable::Currencies, 0, row_data)
{}

CurrencyEntry::CurrencyEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Currencies, row_id, row_data)
{}

AirportEntry::AirportEntry()
    : Row(DbTable::Airports, 0)
{}

AirportEntry::AirportEntry(const RowData_T &row_data)
    : Row(DbTable::Airports, 0, row_data)
{}

AirportEntry::AirportEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Airports, row_id, row_data)
{}

} // namespace OPL
