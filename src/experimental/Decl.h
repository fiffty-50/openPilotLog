#ifndef DECL_H
#define DECL_H

#include <QString>
#include <QPair>
#include <QMap>
#include <QObject>

namespace experimental {

using ColName = QString;
using ColData = QString;
using TableName = QString;
using RowId = int;

using TableNames = QStringList;
/// [G]: May lead to some confusion. TableData suggest data for the entire table.
/// but in reallity it is data per column *of single row* (unless i misunderstand)
using TableData = QMap<ColName, ColData>;
using ColumnData = QPair<ColName, ColData>;
using ColumnNames = QStringList;
using TableColumns = QMap<TableName, ColumnNames>;

/// [G]: Needs some work. Inheriting from QPair may be helpful but
/// may also be overkill. Lets determine the specific uses of DataPosition
/// and provide our own interface i would say.
struct DataPosition : QPair<TableName, RowId> {
    TableName tableName;
    RowId rowId;
    DataPosition()
        : tableName(first), rowId(second)
    {}
    DataPosition(TableName table_name, RowId row_id)
        : QPair<TableName, RowId>::QPair(table_name, row_id),
          tableName(first), rowId(second)
    {}
    DataPosition(const DataPosition& other) = default;
    DataPosition& operator=(const DataPosition& other) = default;
};
auto const DEFAULT_PILOT_POSITION = DataPosition("pilots", 0);

}

#endif // DECL_H
