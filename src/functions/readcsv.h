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
#ifndef READCSV_H
#define READCSV_H

#include "src/opl.h"
#include<QtCore>

namespace CSV {

/*!
 * \brief readCSV reads from a CSV file
 * \param filename - QString to csv file.
 * \return QVector<QStringList> of the CSV data, where each QStringList is one column of the input file
 */
static inline QVector<QStringList> readCSVasColumns(const QString &filename)
{
    QFile csvfile(filename);
    csvfile.open(QIODevice::ReadOnly);
    QTextStream stream(&csvfile);

    QVector<QStringList> values;

    //Read CSV headers and create QStringLists accordingly
    QString line = stream.readLine();
    auto items = line.split(",");
    for (int i = 0; i < items.length(); i++) {
        QStringList list;
        list.append(items[i]);
        values.append(list);
    }
    //Fill QStringLists with data
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        auto items = line.split(",");
        for (int i = 0; i < values.length(); i++) {
            values[i].append(items[i]);
        }
    }
    return values;
}

/*!
 * \brief readCsvAsRows reads from CSV
 * \param file_name input file path
 * \return QVector<QStringList> of the CSV data, where each QStringList is one row of the input file
 */
static inline QVector<QStringList> readCsvAsRows(const QString &file_name)
{
    QFile csvfile(file_name);
    csvfile.open(QIODevice::ReadOnly);
    QTextStream stream(&csvfile);

    QVector<QStringList> csv_rows;

    // Read each line
    while (!stream.atEnd()) {
        const QString line = stream.readLine();
        csv_rows.append(line.split(','));
    }
    return csv_rows;
}

/*!
 * \brief writeCsv write to a CSV file
 * \param output file Path
 * \param rows
 * \return
 */
static inline bool writeCsv(const QString &fileName, const QVector<QVector<QString>> &rows)
{
    QFile csvFile(fileName);
    if(!csvFile.open(QIODevice::ReadWrite | QFile::Text))
        return false;
    QTextStream stream(&csvFile);

    // write each line
    for(const auto &line : rows) {
        for(int i = 0; i < line.size() - 1; i++) {
            stream << line[i] + QLatin1Char(',');
        }
        stream << line.last() + QStringLiteral("\n");
    }
    return true;
}

} // namespace CSV

#endif // READCSV_H
