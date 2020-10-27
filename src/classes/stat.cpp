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
#include "stat.h"


/*!
 * \brief stat::totalTime Looks up Total Blocktime in the flights database
 * \param yearType - Whether the calculation is based on total time, last
 * calendar year or the last rolling year
 * \return Amount of Total Block Time in minutes
 */
QString stat::totalTime(yearType yt)
{
    QString query;
    QDate start;
    QString startdate;

    switch (yt) {
    case stat::allYears:
        query = "SELECT SUM(tblk) FROM flights";
        break;
    case stat::calendarYear:
        start.setDate(QDate::currentDate().year(),1,1);
        startdate = start.toString(Qt::ISODate);
        startdate.append(QLatin1Char('\''));
        startdate.prepend(QLatin1Char('\''));
        query = "SELECT SUM(tblk) FROM flights WHERE doft >= " + startdate;
        break;
    case stat::rollingYear:
        start = QDate::fromJulianDay(QDate::currentDate().toJulianDay() - 365);
        startdate = start.toString(Qt::ISODate);
        startdate.append(QLatin1Char('\''));
        startdate.prepend(QLatin1Char('\''));
        query = "SELECT SUM(tblk) FROM flights WHERE doft >= " + startdate;
        break;}

    QVector<QString> result = db::customQuery(query, 1);

    if(!result.isEmpty()){
        return result[0];
    }else{
        return QString();
    }
}

/*!
 * \brief stat::currencyTakeOffLanding Returns the amount of Take Offs and
 * Landings performed in the last x days. Normally, 90 would be used. (EASA)
 * \param days Number of days to check
 * \return {TO,LDG}
 */
QVector<QString> stat::currencyTakeOffLanding(int days)
{
    QDate start = QDate::fromJulianDay(QDate::currentDate().toJulianDay() - days);
    QString startdate = start.toString(Qt::ISODate);
    startdate.append(QLatin1Char('\''));
    startdate.prepend(QLatin1Char('\''));


    QString query = "SELECT SUM(flights.TOday) + SUM(flights.TOnight) AS 'TO', "
                    "SUM(flights.LDGday) + SUM(flights.LDGnight) AS 'LDG' "
                    "FROM flights "
                    "WHERE doft >= " + startdate;
    QVector<QString> result = db::customQuery(query, 2);

    if(!result.isEmpty()){
        return result;
    }else{
        return QVector<QString>();
    }

}
