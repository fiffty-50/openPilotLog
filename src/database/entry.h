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
#ifndef ENTRY_H
#define ENTRY_H

#include <QCoreApplication>
#include "src/database/db.h"
#include "src/database/dbinfo.h"
/*!
 * \brief The entry class is the base class for database entries.
 * It can be seen as a row in a table within the database.
 *
 */
class entry
{
public:
    entry();
    entry(QString table, int row);
    entry(QString table, QMap<QString, QString> newData);

    QPair   <QString,int>       position = QPair<QString,int>();     // Position within the database, i.e. <table,row>
    QVector <QString>           columns  = QVector<QString>();       // The columns within the table
    QMap    <QString,QString>   data     = QMap<QString,QString>();  // Tha data to fill that table, <column,value>

    void setData(const QMap<QString, QString> &value);

    bool commit();
    bool remove();
    bool exists();


    // Debug functionality
    void print();
    QString debug();
    operator QString() { return debug(); } //overload for compatibility with qDebug()

private:

    bool insert();
    bool update();
};

#endif // ENTRY_H
