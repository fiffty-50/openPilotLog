/*
 *openPilot Log - A FOSS Pilot Logbook Application
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
#ifndef AENTRY_H
#define AENTRY_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QPair>
#include <QVariant>

#include "src/database/declarations.h"

// [G]: Define what data is public and what not. For objects such as
// DataPosition which are consumable its no biggy. Are entries the same?
// If so we could avoid getters and setters
// [F]: In the way we are using the entries atm, we do access and edit the
// TableData quite frequently. Maybe the data could be public, but the
// position private? Except for creating a new entry, the position should
// never really be changed.
/*!
 * \brief The Entry class encapsulates table metadata(table name, row id)
 *  and data for new and existing entries in the database to operate on.
 */
class AEntry {
protected:
    DataPosition position;
public:
    RowData tableData;
public:
    AEntry() = delete; // Demand specificity from default constructor
    AEntry(const AEntry&) = default;
    AEntry& operator=(const AEntry&) = default;
    AEntry(DataPosition position_);
    AEntry(RowData table_data);
    AEntry(DataPosition position_, RowData table_data);

    void setData(RowData table_data);
    void setPosition(DataPosition position_);

    const DataPosition& getPosition();
    const RowData& getData();

};

#endif // AENTRY_H
