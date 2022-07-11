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
#ifndef AENTRY_H
#define AENTRY_H

#include <QString>
#include <QStringList>
#include <QHash>
#include <QPair>
#include <QVariant>

#include "src/database/databasetypes.h"

/*!
 * \brief The Entry class encapsulates table metadata(table name, row id)
 *  and data for new and existing entries in the database to operate on.
 */
class AEntry {
protected:
    DataPosition position;
public:
    RowData_T tableData;
public:
    AEntry() = delete;
    AEntry(const AEntry&) = default;
    AEntry& operator=(const AEntry&) = default;
    AEntry(DataPosition position_);
    AEntry(RowData_T table_data);
    AEntry(DataPosition position_, RowData_T table_data);

    void setData(RowData_T table_data);
    void setPosition(DataPosition position_);

    const DataPosition& getPosition() const;
    const TableName_T &getTableName() const { return position.tableName; }
    const RowId_T &getRowId() const { return position.rowId; }
    const RowData_T& getData() const;

    /*!
     * \brief operator QString provides compatibilty with QDebug() - prints
     * the tableData in a readable formatting to stdout
     */
    operator QString() const;

};

#endif // AENTRY_H
