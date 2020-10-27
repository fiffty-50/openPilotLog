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
#ifndef DB_H
#define DB_H

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QDir>
#include <QDebug>

class sql
{
public:
    enum tableName {flights, pilots, settings };
    enum queryType {select, update };
    enum matchType {exactMatch, partialMatch};
};


class db
{
public:

    static void connect();

    static QString sqliteversion();

    static QVector<QString> getColumnNames(QString table);


    static bool exists(QString column, QString table, QString checkColumn, QString value, sql::matchType match);

    static QString singleSelect(QString column, QString table, QString checkColumn, QString value, sql::matchType match);

    static QVector<QString> multiSelect(QVector<QString> columns, QString table, QString checkColumn, QString value, sql::matchType match);

    static QVector<QString> multiSelect(QVector<QString> columns, QString table);

    static bool singleUpdate(QString table, QString column, QString value, QString checkColumn, QString checkvalue, sql::matchType match);

    static bool deleteRow(QString table, QString column, QString value, sql::matchType match);

    static QVector<QString> customQuery(QString query, int returnValues);

};

#endif // DB_H
