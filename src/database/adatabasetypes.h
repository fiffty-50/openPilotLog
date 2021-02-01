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
#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include <QtCore>
#include "src/testing/adebug.h"

/// \todo Short descriptions
using RowId_T = int;
using PilotName_T = QString;
using PilotRowId_T = RowId_T;
using TailRegistration_T = QString;
using TailId_T = RowId_T;
using AirportICAO_T = QString;
using AirportIATA_T = QString;
using AirportName_T = QString;
using AirportId_T = RowId_T;
using ColName_T = QString;
using ColData_T = QVariant;
using TableName_T = QString;

using TableNames_T = QStringList;
using RowData_T = QMap<ColName_T, ColData_T>;
using ColumnData_T = QPair<ColName_T, ColData_T>;
using ColumnNames_T = QStringList;
using TableColumns_T = QMap<TableName_T, ColumnNames_T>;

struct DataPosition {
    TableName_T tableName;
    RowId_T rowId;
    DataPosition()
        : tableName(TableName_T())
    {};
    DataPosition(TableName_T table_name, RowId_T row_id)
        : tableName(table_name), rowId(row_id)
    {};

    DataPosition(const DataPosition& other) = default;
    DataPosition& operator=(const DataPosition& other) = default;

    REPR(DataPosition,
         "tableName=" + object.tableName
         + ", rowId=" + QString::number(object.rowId)
         )
};

#endif // DECLARATIONS_H
