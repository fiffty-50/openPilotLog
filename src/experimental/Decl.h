#ifndef DECL_H
#define DECL_H

#include <QString>
#include <QPair>
#include <QMap>

namespace experimental {

using ColName = QString;
using ColData = QString;
using TableName = QString;
using RowId = int;

using TableNames = QStringList;
using TableData = QMap<ColName, ColData>;
using ColumnData = QPair<ColName, ColData>;
using ColumnNames = QStringList;
using TableColumns = QMap<TableName, ColumnNames>;

struct DataPosition : QPair<TableName, RowId> {
    TableName& tableName;
    RowId& rowId;
    DataPosition()
        : QPair<TableName, RowId>::QPair(), tableName(first), rowId(second)
    {}
    DataPosition(TableName table_name, RowId row_id)
        : QPair<TableName, RowId>::QPair(table_name, row_id),
          tableName(first), rowId(second)
    {}
    DataPosition(const DataPosition& other) = default;
    void operator=(const DataPosition& other)
    {
        tableName = other.tableName;
        rowId = other.rowId;
    }
};

auto const DEFAULT_PILOT_POSITION = DataPosition("pilots", 0);

}

#endif // DECL_H
