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
#include "flight.h"
#include "src/testing/adebug.h"

Flight::Flight()
{

}

Flight::Flight(int flight_id)
{
    //retreive database layout
    const auto dbContent = DbInfo();

    auto table = QLatin1String("flights");


    //Check database for row id
    QString statement = "SELECT COUNT(*) FROM " + table + " WHERE _rowid_=" + QString::number(flight_id);
    QSqlQuery q(statement);
    q.next();
    int rows = q.value(0).toInt();
    if (rows == 0) {
        DEB("No Entry found for row id: " << flight_id );
        position.second = 0;
    } else {
        DEB("Retreiving data for row id: " << flight_id);
        QString statement = "SELECT * FROM " + table + " WHERE _rowid_=" + QString::number(flight_id);

        QSqlQuery q(statement);
        q.exec();
        q.next();
        for (int i = 0; i < dbContent.format.value(table).length(); i++) {
            data.insert(dbContent.format.value(table)[i], q.value(i).toString());
        }

        error = q.lastError().text();
        if (error.length() > 2) {
            DEB("Error: " << q.lastError().text());
            position.second = 0;
            position.first = "invalid";
        } else {
            position.second = flight_id;
            position.first = "flights";
        }
    }
}

Flight::Flight(QMap<QString, QString> newData)
{
    QString table = "flights";

    //retreive database layout
    const auto dbContent = DbInfo();
    columns = dbContent.format.value(table);

    //Check validity of newData
    QVector<QString> badkeys;
    QMap<QString, QString>::iterator i;
    for (i = newData.begin(); i != newData.end(); ++i) {
        if (!columns.contains(i.key())) {
            DEB(i.key() << "Not in column list for table " << table << ". Discarding.");
            badkeys << i.key();
        }
    }
    for (const auto &var : badkeys) {
        newData.remove(var);
    }
    data = newData;
    position.first = table;
    position.second = 0;
}
