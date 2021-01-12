#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include <QtCore>
#include "src/oplconstants.h"
#include "src/testing/adebug.h"

/*!
 * \brief An alias for QString
 *
 * Very long description *with* **markdown?**
 * - - -
 * # Header
 */
using ColName = QString;
using ColData = QVariant;
using TableName = QString;
using RowId = int;

using TableNames = QStringList;
using RowData = QMap<ColName, ColData>;
using ColumnData = QPair<ColName, ColData>;
using ColumnNames = QStringList;
using TableColumns = QMap<TableName, ColumnNames>;

struct DataPosition {
    TableName tableName;
    RowId rowId;
    DataPosition()
        : tableName(TableName())
    {};
    DataPosition(TableName table_name, RowId row_id)
        : tableName(table_name), rowId(row_id)
    {};

    DataPosition(const DataPosition& other) = default;
    DataPosition& operator=(const DataPosition& other) = default;

    REPR(DataPosition,
         "tableName=" + object.tableName
         + ", rowId=" + QString::number(object.rowId)
         )
};

// Default Positions
static auto const DEFAULT_FLIGHT_POSITION   = DataPosition(Opl::Db::TABLE_FLIGHTS, 0);
static auto const DEFAULT_PILOT_POSITION    = DataPosition(Opl::Db::TABLE_PILOTS, 0);
static auto const DEFAULT_TAIL_POSITION     = DataPosition(Opl::Db::TABLE_TAILS, 0);
static auto const DEFAULT_AIRCRAFT_POSITION = DataPosition(Opl::Db::TABLE_AIRCRAFT, 0);

#endif // DECLARATIONS_H
