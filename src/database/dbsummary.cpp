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
#include "dbsummary.h"
#include "QtCore/qstringliteral.h"
#include "QtSql/qsqldatabase.h"
#include "QtSql/qsqlquery.h"

namespace OPL {

const QMap<DbSummaryKey, QString> DbSummary::databaseSummary(const QString &db_path)
{
    const QString connection_name = QStringLiteral("summary_connection");
    QMap<DbSummaryKey, QString> return_values;
    { // scope for a temporary database connection, ensures proper cleanup when removeDatabase() is called.
        //DEB << "Adding temporary connection to database:" << db_path;
        QSqlDatabase temp_database = QSqlDatabase::addDatabase(SQLITE_DRIVER, connection_name); // Don't use default connection
        temp_database.setDatabaseName(db_path);
        if (!temp_database.open())
            return {};

        QSqlQuery query(temp_database); // Query object using the temporary connection
        DbSummaryKey key;  // Used among the queries for verbosity... and sanity

        const QVector<QPair<DbSummaryKey, QString>> key_table_pairs = {
            {DbSummaryKey::total_flights, QStringLiteral("flights")},
            {DbSummaryKey::total_tails, QStringLiteral("tails")},
            {DbSummaryKey::total_pilots, QStringLiteral("pilots")}
        };
        // retreive amount of flights, tails and pilots
        for (const auto & pair : key_table_pairs) {
            query.prepare(QLatin1String("SELECT COUNT (*) FROM ") + pair.second);
            query.exec();
            key = pair.first;
            if (query.first()){
                return_values[key] = query.value(0).toString();
            }
            else{
                return_values[key] = QString();
            }
        }
        // retreive date of last flight
        query.prepare(QStringLiteral("SELECT MAX(doft) FROM flights"));
        query.exec();
        key = DbSummaryKey::last_flight;
        if (query.first()){
            return_values[key] = query.value(0).toString();
        }
        else {
            return_values[key] = QString();
        }
        // retreive total flight time as a string "hh:mm"
        query.prepare(QStringLiteral("SELECT "
                                     "printf(\"%02d\",CAST(SUM(tblk) AS INT)/60)"
                                     "||':'||"
                                     "printf(\"%02d\",CAST(SUM(tblk) AS INT)%60) FROM flights"));
        key = DbSummaryKey::total_time;
        query.exec();
        if (query.first()){
            return_values[key] = query.value(0).toString();
        }
        else {
            return_values[key] = QString();
        }
    }

    QSqlDatabase::removeDatabase(connection_name); // cleanly removes temp connection without leaks since query+db are out of scope

    return return_values;
}

const QString DbSummary::summaryString(const QString &db_path)
{
    auto summary_map = databaseSummary(db_path);
    QString out = QLatin1String("<table>");
    out.append(tr("<tr><td>Total Time: </td><td>%1</td>").arg(summary_map[DbSummaryKey::total_time]));
    out.append(tr("<tr><td>Last Flight: </td><td>%1</td>").arg(summary_map[DbSummaryKey::last_flight]));
    out.append(tr("<tr><td>Number of flights: </td><td>%1</td>").arg(summary_map[DbSummaryKey::total_flights]));
    out.append(tr("<tr><td>Number of aircraft: </td><td>%1</td>").arg(summary_map[DbSummaryKey::total_tails]));
    out.append(tr("<tr><td>Number of Pilots: </td><td>%1</td>").arg(summary_map[DbSummaryKey::total_pilots]));
    out.append("</table>");

    return out;
}

} // namespace OPL
