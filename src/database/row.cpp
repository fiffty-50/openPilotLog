/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#include "row.h"

namespace OPL {

Row::Row(DbTable table_name, const QList<QString> *fields)
    : m_table(table_name), m_fields(fields), m_rowId(0)
{
    // make sure all fields are correctly initialized to NULL
    m_rowData.reserve(m_fields->size());

    for (const auto &field : *m_fields) {
        m_rowData.insert(field, OPL::NullData);
    }
}; // Used for a new entry

Row::Row(OPL::DbTable table_name, int row_id, const RowData_T &row_data,
         const QList<QString> *fields)
    : m_table(table_name), m_rowId(row_id), m_rowData(row_data)
{
}

const RowData_T &Row::getData() const { return m_rowData; }

void Row::setData(const RowData_T &value) { m_rowData = value; }

int Row::getRowId() const { return m_rowId; }

void Row::setRowId(int value) { m_rowId = value; }

OPL::DbTable Row::getTable() const { return m_table; }

const QString Row::getPosition() const
{
    return QString("Table: %1 / RowID: %2")
        .arg(OPL::GLOBALS->getDbTableName(m_table), QString::number(m_rowId));
}

// Used for debugging
// OPL::Row::operator QString() const
// {
//     if (!isValid()) {
//         return QStringLiteral("Invalid Row: ") + getPosition();
//     }
//     constexpr int columnWidth = 14;
//     constexpr int itemsPerRow = 3;

//     const QString resetColor     = "\033[m";
//     const QString highlightColor = "\033[35m";

//     QString out;
//     out.reserve(1024);

//     out += "[Entry Data]: " + getPosition() + "\n";

//     int itemCount = 0;

//     for (auto it = m_rowData.cbegin(); it != m_rowData.cend(); ++it) {
//         const QString key   = it.key();
//         const QString value = it.value().toString();

//         QString paddedKey = key;
//         paddedKey += QLatin1Char(':');
//         paddedKey = paddedKey.leftJustified(columnWidth);

//         const QString displayVal  = value.isEmpty() ? "-NULL-" : value;
//         const QString paddedValue = displayVal.leftJustified(columnWidth);

//         out += "\t" % resetColor % paddedKey % highlightColor % paddedValue;

//         if (++itemCount % itemsPerRow == 0) out += "\n";
//     }

//     out += "\n";
//     QTextStream(stdout) << out;
//     return QString();
// }

OPL::Row::operator QString() const
{
    constexpr int columnWidth = 14;
    constexpr int itemsPerRow = 3;

    const QString resetColor     = "\033[m";
    const QString highlightColor = "\033[35m";
    const QString errorColor     = "\033[31m"; // red

    QString out;
    out.reserve(1024);

    if (!isValid()) {
        out += errorColor;
        out += "Invalid Row: " + getPosition() + "\n";
        out += resetColor;
    }

    out += "[Entry Data]: " + getPosition() + "\n";

    int itemCount = 0;

    for (auto it = m_rowData.cbegin(); it != m_rowData.cend(); ++it) {
        const QString key   = it.key();
        const QString value = it.value().toString();

        QString paddedKey = key;
        paddedKey += QLatin1Char(':');
        paddedKey = paddedKey.leftJustified(columnWidth);

        const QString displayVal  = value.isEmpty() ? "-NULL-" : value;
        const QString paddedValue = displayVal.leftJustified(columnWidth);

        out += "\t" % resetColor % paddedKey % highlightColor % paddedValue;

        if (++itemCount % itemsPerRow == 0)
            out += "\n";
    }

    out += "\n";
    QTextStream(stdout) << out;
    return out;
}


} // namespace OPL
