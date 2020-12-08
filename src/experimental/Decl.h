#ifndef DECL_H
#define DECL_H

#include <QString>
#include <QPair>
#include <QMap>

namespace experimental {

//using ColName = QString;
//using ColData = QString;
//using TableName = QString;
//using RowId = int;

//using TableNames = QStringList;
//using TableData = QMap<ColName, ColData>;
//using ColumnData = QPair<ColName, ColData>;
//using ColumnNames = QStringList;
//using TableColumns = QMap<TableName, ColumnNames>;

struct DataPosition : QPair<QString, int> {
    QString& tableName;
    int& rowId;
    DataPosition()
        : QPair<QString, int>::QPair(), tableName(first), rowId(second)
    {}
    DataPosition(QString tn, int ri)
        : QPair<QString, int>::QPair(tn, ri), tableName(first), rowId(second)
    {}
};

auto const DEFAULT_PILOT_POSITION = QPair<QString, int>("pilots", 0);

}

#endif // DECL_H
