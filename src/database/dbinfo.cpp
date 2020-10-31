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
#include "dbinfo.h"
// Debug Makro
#define DEB(expr) \
    qDebug() << "dbInfo ::" << __func__ << "\t" << expr
dbInfo::dbInfo()
{
    QSqlDatabase db = QSqlDatabase::database("qt_sql_default_connection");
    tables = db.tables().toVector();
    getColumnNames();
    sqliteversion();
}

/*!
 * \brief dbInfo::getColumnNames Looks up column names for all tables
 * in the database.
 */
void dbInfo::getColumnNames()
{
    QSqlDatabase db = QSqlDatabase::database("qt_sql_default_connection");
    QVector<QString> columnNames;
    for(const auto& table : tables){
        columnNames.clear();
        QSqlRecord fields = db.record(table);
        for(int i = 0; i < fields.count(); i++){
            columnNames << fields.field(i).name();
            format.insert(table,columnNames);
        }
    }
}

/*!
 * \brief db::sqliteversion queries database version.
 */
void dbInfo::sqliteversion()
{
    QSqlQuery q;
    q.prepare("SELECT sqlite_version()");
    q.exec();
    q.next();
    version = q.value(0).toString();
}
