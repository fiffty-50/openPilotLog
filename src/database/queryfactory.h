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
#ifndef QUERYFACTORY_H
#define QUERYFACTORY_H

#include "src/database/entries/row.h"
#include "src/opl.h"
#include <QObject>
#include <QSqlError>
#include <QSqlQuery>

namespace OPL {

class QueryFactory : public QObject {
    Q_OBJECT
  public:
    explicit QueryFactory(QObject *parent = nullptr);

    QSqlQuery exists(const Row &row);
    QSqlQuery exists(DbTable table, int row_id);

    /*!
     * \brief Returns a prepared Query that updates an existing Row
     */
    QSqlQuery update(const Row &data);

    /*!
     * \brief Returns a prepared Query that inserts a new row
     * \details OPL::Row contains all the data needed to construct a Query
     *"INSERT INTO table (columns) VALUES (values)". If a value for a given
     * column is empty, NULL is commited.
     */
    QSqlQuery insert(const Row &row);

    /*!
     * \brief Returns a prepared query to insert values into a table
     * \details This query prepares an insert query
     * "INSERT INTO table (columns) VALUES (:c1, :c2,...)" where the values
     * still have to be bound. This query is useful when batch-inserting values
     * stored in a JSON file
     * */
    QSqlQuery insert(DbTable table, const QStringList &columns);

    /*!
     * \brief Returns a prepared Query "DELETE FROM table WHERE ROWID=value"
     */
    QSqlQuery deleteFrom(DbTable table, int row_id);
    /*!
     * \brief Returns a prepared Query "DELETE FROM table WHERE column=value"
     */
    QSqlQuery deleteFromWhere(DbTable table, const QString &column, int value);
    /*!
     * \brief Returns a prepared Query "DELETE FROM table WHERE column=value"
     */
    QSqlQuery deleteFromWhere(DbTable table, const QString &column, const QString &value);

    /*!
     * \brief Returns a prepared Query "SELECT * FROM table WHERE ROWID=value"
     */
    QSqlQuery selectFrom(DbTable table, int row_id);

    /*!
     * \brief Returns a prepared Query "SELECT * FROM table WHERE filter_column=value"
     */
    QSqlQuery selectFromWhere(DbTable table, const QString &filter_column, int row_id);

    /*!
     * \brief Returns the last entry that has been added to the specified table (highest ROWID)
     */
    QSqlQuery selectMaxRowId(DbTable table);

    /*!
     * \brief Returns a prepared Query "SELECT ROWID FROM table WHERE column=?"
     */
    QSqlQuery getForeignKeyConstraint(DbTable table, const QString &column, int value);

    /*!
     * \brief Returns a prepared Query "SELECT * FROM table"
     */
    QSqlQuery getTable(DbTable table);

  private:
    const static inline auto COL_EVENT_ID  = QStringLiteral("event_id");
    const static inline auto COL_FLIGHT_ID = QStringLiteral("flight_id");
    const static inline auto QUOTE         = QStringLiteral("\"");
    constexpr static auto QMARK            = QLatin1Char('?');
    constexpr static auto COMMA            = QLatin1Char(',');
    constexpr static auto COLON            = QLatin1Char(':');
    constexpr static auto EQUALS           = QLatin1Char('=');

    const static inline auto Q_DELETE_WHERE = QStringLiteral("DELETE FROM %1 WHERE %2=?");
    const static inline auto Q_DELETE       = QStringLiteral("DELETE FROM %1 WHERE ROWID=?");
    const static inline auto Q_UPDATE       = QStringLiteral("UPDATE %1 SET %2 WHERE ROWID=?");
    const static inline auto Q_INSERT       = QStringLiteral("INSERT INTO %1 (%2) VALUES (%3)");
    const static inline auto Q_EXISTS       = QStringLiteral("SELECT 1 FROM %1 WHERE ROWID=?");
    const static inline auto Q_SELECT       = QStringLiteral("SELECT * FROM %1 WHERE ROWID=?");
    const static inline auto Q_SELECT_WHERE = QStringLiteral("SELECT * FROM %1 WHERE %2=? ");
    const static inline auto Q_LAST_ENTRY   = QStringLiteral("SELECT MAX(ROWID) FROM %1");
    const static inline auto Q_FOREIGN_KEY  = QStringLiteral("SELECT ROWID FROM %1 WHERE %2=?");
    const static inline auto Q_GET_TABLE    = QStringLiteral("SELECT * FROM %1");

    static inline QString tableName(DbTable table) { return GLOBALS->getDbTableName(table); }
    static inline QSqlQuery prepare(const QString &query)
    {
        QSqlQuery q;
        q.prepare(query);
        q.setForwardOnly(true);
        return q;
    }
};

Q_GLOBAL_STATIC(QueryFactory, QUERIES);

} // namespace OPL

#endif // QUERYFACTORY_H
