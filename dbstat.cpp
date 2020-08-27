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
#include "dbstat.h"
#include "dbman.cpp"

/*!
 * \brief retreiveTotalTime Looks up Total Blocktime in the flights database
 * \return Amount of Total Block Time in blockminutes
 */
QString dbStat::retreiveTotalTime()
{
    QSqlQuery query;
    query.prepare("SELECT SUM(tblk) FROM flights");
    query.exec();

    QString result;
    while (query.next()){
      result = query.value(0).toString();
    }
    return result;
}

/*!
 * \brief retreiveTotalTimeThisCalendarYear Looks up Total Block Time in
 * the current calendar year
 * \return Total Amount of Blockminutes
 */
QString dbStat::retreiveTotalTimeThisCalendarYear()
{
    QDate today = QDate::currentDate();
    QDate start;
    start.setDate(today.year(),1,1);
    QString startdate = start.toString(Qt::ISODate);

    QSqlQuery query;
    query.prepare("SELECT SUM(tblk) FROM flights WHERE doft >= ?");
    query.addBindValue(startdate);
    query.exec();

    QString result;
    while (query.next()){
        result = query.value(0).toString();
    }

    qDebug() << "db::retreiveTotalTimeThisCalendarYear: Total minutes: " << result;
    return result;
}

/*!
 * \brief retreiveTotalTimeRollingYear Looks up Total Time in the last 365 days.
 * \return Total Blockminutes
 */
QString dbStat::retreiveTotalTimeRollingYear()
{
    QDate start = QDate::fromJulianDay(QDate::currentDate().toJulianDay() - 365);
    QString startdate = start.toString(Qt::ISODate);

    QSqlQuery query;
    query.prepare("SELECT SUM(tblk) FROM flights WHERE doft >= ?");
    query.addBindValue(startdate);
    query.exec();

    QString result;
    while (query.next()){
        result = query.value(0).toString();
    }

    qDebug() << "db::retreiveTotalTimeRollingYear: Total minutes: " << result;
    return result;
}

/*!
 * \brief retreiveCurrencyTakeoffLanding Looks up the amount of Take Offs and
 * Landings performed in the last 90 days.
 * \return {TO,LDG}
 */
QVector<QString> dbStat::retreiveCurrencyTakeoffLanding()
{
    QDate start = QDate::fromJulianDay(QDate::currentDate().toJulianDay() - 90);

    QSqlQuery query;
    query.prepare("SELECT "
                  "SUM(extras.TOday + extras.TOnight) AS 'TakeOFF', "
                  "SUM(extras.LDGday + extras.LDGnight) AS 'Landing' "
                  "FROM flights "
                  "INNER JOIN extras on flights.id = extras.extras_id "
                  "WHERE doft >= ?");
    query.addBindValue(start.toString(Qt::ISODate));
    query.exec();

    QVector<QString> result(2,"0"); // make sure to return a valid vector even if result 0
    while (query.next()){
        result.insert(0,query.value(0).toString());
        result.insert(1,query.value(1).toString());
    }
    qDebug() << "db::retreiveCurrencyTakeoffLanding: " << result[0] << " TO, " <<result[1] << " LDG";
    return result;
}
