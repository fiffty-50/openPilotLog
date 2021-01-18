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
#include "areadcsv.h"

/*!
 * \brief aReadCSV reads from a CSV file
 * \param filename - QString to csv file.
 * \return QVector<QStringList> of the CSV data, where each QStringList is one column of the input file
 */
QVector<QStringList> aReadCsv(QString filename)
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
