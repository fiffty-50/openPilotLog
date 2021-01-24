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
#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include <QtCore>
#include "src/testing/adebug.h"

// [G]: TODO. Break apart these aliases to their
// corresponding sub section of the program
// eg DB related to ADatabase.h and so on

using RowId_t = int;
using PilotName_t = QString;
using PilotRowId_t = RowId_t;
using TailRgstr_t = QString;
using TailId_t = RowId_t;
using ArprtICAO_t = QString;
using ArprtIATA_t = QString;
using ArprtName_t = QString;
using ArprtId_t = RowId_t;
using ColName_t = QString;
using ColData_t = QVariant;
using TableName_t = QString;
using RowId_t = int;

using TableNames_t = QStringList;
using RowData_t = QMap<ColName_t, ColData_t>;
using ColumnData_t = QPair<ColName_t, ColData_t>;
using ColumnNames_t = QStringList;
using TableColumns_t = QMap<TableName_t, ColumnNames_t>;

using ForeignKey_t = int;

struct DataPosition {
    TableName_t tableName;
    RowId_t rowId;
    DataPosition()
        : tableName(TableName_t())
    {};
    DataPosition(TableName_t table_name, RowId_t row_id)
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
