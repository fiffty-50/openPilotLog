#ifndef DECL_H
#define DECL_H

#include <QString>
#include <QPair>
#include <QMap>
#include <QObject>

/*!
 * \brief Temporary namespace for experimental features.
 * Will be removed in later versions.
 * \todo Remove this namespace (and learn about some nice \something of doxygen)
 * Decide documentation style. https://www.doxygen.nl/manual/docblocks.html#specialblock
 */
namespace experimental {


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
// [G]: May lead to some confusion. TableData suggest data for the entire table.
// but in reallity it is data per column *of single row* (unless i misunderstand)
// [F]: That's correct. We could maybe call it EntryData or RowData?
using TableData = QMap<ColName, ColData>;
using ColumnData = QPair<ColName, ColData>;
using ColumnNames = QStringList;
using TableColumns = QMap<TableName, ColumnNames>;

// [G]: Needs some work. Inheriting from QPair may be helpful but
// may also be overkill. Lets determine the specific uses of DataPosition
// and provide our own interface i would say.
// [F]: Good idea! Implementing something similar to first and second methods
// of QPair would be useful to carry over, or some other way of quickly and
// unambiguously accessing the elements.
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
auto const DEFAULT_PILOT_POSITION    = DataPosition(QStringLiteral("pilots"), 0);
auto const DEFAULT_TAIL_POSITION     = DataPosition(QStringLiteral("tails"), 0);
auto const DEFAULT_AIRCRAFT_POSITION = DataPosition(QStringLiteral("aircraft"), 0);
auto const DEFAULT_FLIGHT_POSITION   = DataPosition(QStringLiteral("flights"), 0);

}

#endif // DECL_H
