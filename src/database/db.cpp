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
#include "db.h"
#include "dbinfo.h"
#include "debug.h"

/*!
 * \brief Db::iconnect - see Db::connect
 */
void Db::iconnect()
{
    const QString driver("QSQLITE");

    if (QSqlDatabase::isDriverAvailable(driver)) {

        QDir directory("data");
        QString databaseLocation = directory.filePath("logbook.db");
        QSqlDatabase db = QSqlDatabase::addDatabase(driver);
        db.setDatabaseName(databaseLocation);

        if (!db.open()) {
            DEB("DatabaseConnect - ERROR: " << db.lastError().text());
        } else {
            DEB("Database connection established.");
            //Enable foreign key restrictions
            QSqlQuery query("PRAGMA foreign_keys = ON;");
        }
    } else {
        DEB("DatabaseConnect - ERROR: no driver " << driver << " available");
    }
}

void Db::idisconnect()
{
    auto db = Db::Database();
    db.close();
    db.removeDatabase(db.connectionName());
    DEB("Database connection closed.");
}

QSqlDatabase Db::iDatabase()
{
    auto db = QSqlDatabase::database("qt_sql_default_connection");
    return db;
}
/*!
 * \brief Db::iexists - see Db::exists
 */
bool Db::iexists(QString column, QString table, QString checkColumn, QString value,
                Db::matchType match)
{
    bool output = false;
    QString statement = "SELECT " + column + " FROM " + table + " WHERE " + checkColumn;

    switch (match) {
    case Db::exactMatch:
        statement += " = '" + value + QLatin1Char('\'');
        break;
    case Db::partialMatch:
        value.append(QLatin1Char('%'));
        value.prepend(QLatin1Char('%'));
        statement.append(" LIKE '" + value + QLatin1Char('\''));
        break;
    }


    QSqlQuery q(statement);
    q.exec();

    if (!q.first()) {
        DEB("No result found. Check Query and Error.");
        DEB("Error: " << q.lastError().text());
    } else {
        output = true;
        if (q.next()) {
            DEB("More than one result in Database for your query");
        }
    }
// Debug:
    q.first();
    q.previous();
    while (q.next()) {
        DEB("Query result: " << q.value(0).toString());
    }
// end of Debug
    return output;
}

/*!
 * \brief Db::isingleSelect - see Db::singleSelect
 */
QString Db::isingleSelect(QString column, QString table, QString checkColumn, QString value,
                         Db::matchType match)
{
    QString statement = "SELECT " + column + " FROM " + table + " WHERE " + checkColumn;
    QString result;

    switch (match) {
    case Db::exactMatch:
        statement += " = '" + value + QLatin1Char('\'');
        break;
    case Db::partialMatch:
        value.append(QLatin1Char('%'));
        value.prepend(QLatin1Char('%'));
        statement.append(" LIKE '" + value + QLatin1Char('\''));
        break;
    }

    QSqlQuery q(statement);
    q.exec();

    if (!q.first()) {
        DEB("No result found. Check Query and Error.");
        DEB("Error: " << q.lastError().text());
        return QString();
    } else {
        result.append(q.value(0).toString());
        if (q.next()) {
            DEB("More than one result in Database for your query");
        }
        return result;
    }
}

/*!
 * \brief Db::imultiSelect - see Db::multiSelect
 */
QVector<QString> Db::imultiSelect(QVector<QString> columns, QString table, QString checkColumn,
                                 QString value, Db::matchType match)
{
    QString statement = "SELECT ";
    for (const auto &column : columns) {
        statement.append(column);
        if (column != columns.last()) {
            statement.append(QLatin1String(", "));
        }
    }
    statement.append(" FROM " + table + " WHERE " + checkColumn);

    switch (match) {
    case Db::exactMatch:
        statement += " = '" + value + QLatin1Char('\'');
        break;
    case Db::partialMatch:
        value.append(QLatin1Char('%'));
        value.prepend(QLatin1Char('%'));
        statement.append(" LIKE '" + value + QLatin1Char('\''));
        break;
    }

    QSqlQuery q(statement);
    q.exec();

    if (!q.first()) {
        DEB("No result found. Check Query and Error.");
        DEB("Error: " << q.lastError().text());
        return QVector<QString>();
    } else {
        q.first();
        q.previous();
        QVector<QString> result;
        while (q.next()) {
            for (int i = 0; i < columns.size() ; i++) {
                result.append(q.value(i).toString());
            }
        }
        return result;
    }
}

/*!
 * \brief Db::imultiSelect - see Db::multiSelect
 */
QVector<QString> Db::imultiSelect(QVector<QString> columns, QString table)
{
    QString statement = "SELECT ";
    for (const auto &column : columns) {
        statement.append(column);
        if (column != columns.last()) {
            statement.append(QLatin1String(", "));
        }
    }
    statement.append(" FROM " + table);

    QSqlQuery q(statement);
    q.exec();

    if (!q.first()) {
        DEB("No result found. Check Query and Error.");
        DEB("Error: " << q.lastError().text());
        return QVector<QString>();
    } else {
        q.first();
        q.previous();
        QVector<QString> result;
        while (q.next()) {
            for (int i = 0; i < columns.size() ; i++) {
                result.append(q.value(i).toString());
            }
        }
        return result;
    }
}

/*!
 * \brief Db::isingleUpdate - see Db::singleUpdate
 */
bool Db::isingleUpdate(QString table, QString column, QString value, QString checkColumn,
                      QString checkvalue, Db::matchType match)
{
    QString statement = "UPDATE " + table;
    statement.append(QLatin1String(" SET ") + column + QLatin1String(" = '") + value);
    statement.append(QLatin1String("' WHERE "));

    switch (match) {
    case Db::exactMatch:
        statement.append(checkColumn + " = '" + checkvalue + QLatin1Char('\''));
        break;
    case Db::partialMatch:
        value.append(QLatin1Char('%'));
        value.prepend(QLatin1Char('%'));
        statement.append(checkColumn + " LIKE '" + checkvalue + QLatin1Char('\''));
        break;
    }

    QSqlQuery q(statement);
    q.exec();
    QString error = q.lastError().text();

    if (error.length() > 1) {
        DEB("Errors have occured: " << error);
        return false;
    } else {
        DEB("Success!");
        return true;
    }
}
/*!
 * \brief Db::icustomQuery - see Db::customQuery
 */
QVector<QString> Db::icustomQuery(QString query, int returnValues)
{
    QSqlQuery q(query);
    q.exec();

    if (!q.first()) {
        DEB("No result found. Check Query and Error.");
        DEB("Error: " << q.lastError().text());
        return QVector<QString>();
    } else {
        q.first();
        q.previous();
        QVector<QString> result;
        while (q.next()) {
            for (int i = 0; i < returnValues ; i++) {
                result.append(q.value(i).toString());
            }
        }
        return result;
    }
}
