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

RowData_T Row::getRowData() const
{
    return rowData;
}

void Row::setRowData(const RowData_T &value)
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

OPL::Row::operator QString() const
{
    if (!isValid()) {
        return QStringLiteral("Invalid Row");
    }
    QString out("\033[32m[Entry Data]:\t\033[m\n");
    int item_count = 0;
    QHash<ColName_T, ColData_T>::const_iterator i;
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

} // namespace OPL
