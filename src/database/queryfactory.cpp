/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
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
#include "queryfactory.h"
#include <QSqlQuery>

namespace OPL {

QueryFactory::QueryFactory(QObject *parent) : QObject{parent} {}

QSqlQuery QueryFactory::exists(const Row &row)
{
    QString query = Q_EXISTS.arg(row.getTableName());
    auto q        = prepare(query);
    q.addBindValue(row.getRowId());

    return q;
}

QSqlQuery QueryFactory::exists(DbTable table, int row_id)
{
    QString query = Q_EXISTS.arg(tableName(table));
    auto q        = prepare(query);
    q.addBindValue(row_id);

    return q;
}

QSqlQuery QueryFactory::deleteFrom(DbTable table, int row_id)
{
    QString query = Q_DELETE.arg(tableName(table));
    auto q        = prepare(query);
    q.addBindValue(row_id);

    return q;
}

QSqlQuery OPL::QueryFactory::deleteFromWhere(DbTable table, const QString &column, int value)
{
    QString query = Q_DELETE_WHERE.arg(tableName(table), column);
    auto q        = prepare(query);
    q.addBindValue(value);

    return q;
}

QSqlQuery OPL::QueryFactory::deleteFromWhere(DbTable table, const QString &column,
                                             const QString &value)
{
    QString query = Q_DELETE_WHERE.arg(tableName(table), column);
    auto q        = prepare(query);
    q.addBindValue(value);

    return q;
}

QSqlQuery QueryFactory::update(const Row &row)
{

    QStringList columns;
    columns.reserve(row.getData().size());
    for (const auto &key : row.getData().keys()) {
        columns << QUOTE % key % QUOTE % EQUALS % QMARK; // "column"=value
    }

    QString query = Q_UPDATE.arg(row.getTableName(), columns.join(COMMA));
    auto q        = prepare(query);

    for (auto it = row.getData().cbegin(); it != row.getData().cend(); ++it) {
        if (it.value().isNull() || it.value().toString() == QString()) {
            q.addBindValue(QVariant(it.value().metaType()));
        }
        else {
            q.addBindValue(it.value());
        }
    }

    q.addBindValue(row.getRowId());

    return q;
}

QSqlQuery QueryFactory::insert(const Row &row)
{
    // Prepare column names and placeholders
    int size = row.getData().size();
    QStringList columns;
    columns.reserve(size);
    QStringList placeholders;
    placeholders.reserve(size);

    for (auto it = row.getData().cbegin(); it != row.getData().cend(); ++it) {
        columns << (QUOTE % it.key() % QUOTE);
        placeholders << QString(QMARK);
    }

    // Build the Query String
    QString query = Q_INSERT.arg(row.getTableName(), columns.join(COMMA), placeholders.join(COMMA));
    auto q        = prepare(query);

    // add bind values
    for (auto it = row.getData().cbegin(); it != row.getData().cend(); ++it) {
        if (it.value().isNull() || it.value().toString() == QString()) {
            q.addBindValue(QVariant(it.value().metaType()));
        }
        else {
            q.addBindValue(it.value());
        }
    }

    return q;
}

QSqlQuery QueryFactory::insert(DbTable table, const QStringList &columns)
{
    // Prepare to build the query string
    constexpr int EST_MAX_SIZE = 256;
    QString query              = QStringLiteral("INSERT INTO %1 (").arg(tableName(table));
    query.reserve(EST_MAX_SIZE);
    QString values = QStringLiteral(") VALUES (");
    values.reserve(EST_MAX_SIZE);

    for (const auto &column : columns) {
        query.append(QUOTE % column % QUOTE % COMMA);
        values.append(COLON % column % COMMA);
    }

    // finish up
    query.chop(1);        // remove last COMMA
    values.chop(1);       // remove last COMMA
    values.append(')');   // add a closing bracket
    query.append(values); // join the two halves

    // return the query object
    return prepare(query);
}

QSqlQuery QueryFactory::selectFrom(DbTable table, int row_id)
{
    QString query = Q_SELECT.arg(tableName(table));

    auto q = prepare(query);
    q.addBindValue(row_id);

    return q;
}

QSqlQuery QueryFactory::selectFromWhere(DbTable table, const QString &filter_column, int row_id)
{
    QString query = Q_SELECT_WHERE.arg(tableName(table), filter_column);

    auto q = prepare(query);
    q.addBindValue(row_id);

    return q;
}

QSqlQuery QueryFactory::selectMaxRowId(DbTable table)
{
    QString query = Q_LAST_ENTRY.arg(tableName(table));
    return prepare(query);
}

QSqlQuery QueryFactory::getForeignKeyConstraint(DbTable table, const QString &column, int value)
{
    QString query = Q_FOREIGN_KEY.arg(tableName(table), column);
    auto q        = prepare(query);
    q.addBindValue(value);
    return q;
}

QSqlQuery QueryFactory::getTable(DbTable table)
{
    QString query = Q_GET_TABLE.arg(tableName(table));
    return prepare(query);
}

} // namespace OPL
