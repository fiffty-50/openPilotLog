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
 * \brief Stat::totalTime Looks up Total Blocktime in the flights database
 * \param yearType - Whether the calculation is based on total time, last
 * calendar year or the last rolling year
 * \return Amount of Total Block Time in minutes
 */
QString Stat::totalTime(yearType yt)
{
    QString query;
    QDate start;
    QString startdate;

    switch (yt) {
    case Stat::allYears:
        query = "SELECT SUM(tblk) FROM flights";
        break;
    case Stat::calendarYear:
        start.setDate(QDate::currentDate().year(), 1, 1);
        startdate = start.toString(Qt::ISODate);
        startdate.append(QLatin1Char('\''));
        startdate.prepend(QLatin1Char('\''));
        query = "SELECT SUM(tblk) FROM flights WHERE doft >= " + startdate;
        break;
    case Stat::rollingYear:
        start = QDate::fromJulianDay(QDate::currentDate().toJulianDay() - 365);
        startdate = start.toString(Qt::ISODate);
        startdate.append(QLatin1Char('\''));
        startdate.prepend(QLatin1Char('\''));
        query = "SELECT SUM(tblk) FROM flights WHERE doft >= " + startdate;
        break;
    }

    QVector<QString> result = Db::customQuery(query, 1);

    if (!result.isEmpty()) {
        return result[0];
    } else {
        return QString();
    }
}

/*!
 * \brief Stat::currencyTakeOffLanding Returns the amount of Take Offs and
 * Landings performed in the last x days. Normally, 90 would be used. (EASA)
 * \param days Number of days to check
 * \return {TO,LDG}
 */
QVector<QString> Stat::currencyTakeOffLanding(int days)
{
    QDate start = QDate::fromJulianDay(QDate::currentDate().toJulianDay() - days);
    QString startdate = start.toString(Qt::ISODate);
    startdate.append(QLatin1Char('\''));
    startdate.prepend(QLatin1Char('\''));


    QString query = "SELECT SUM(flights.TOday) + SUM(flights.TOnight) AS 'TO', "
                    "SUM(flights.LDGday) + SUM(flights.LDGnight) AS 'LDG' "
                    "FROM flights "
                    "WHERE doft >= " + startdate;
    QVector<QString> result = Db::customQuery(query, 2);

    if (!result.isEmpty()) {
        return result;
    } else {
        return QVector<QString>();
    }

}

QVector<QPair<QString, QString>> Stat::totals()
{
    QString statement = "SELECT "
            "printf('%02d',CAST(SUM(tblk) AS INT)/60)||':'||printf('%02d',CAST(SUM(tblk) AS INT)%60) AS 'TOTAL', "
            "printf('%02d',CAST(SUM(tSPSE) AS INT)/60)||':'||printf('%02d',CAST(SUM(tSPSE) AS INT)%60) AS 'SP SE', "
            "printf('%02d',CAST(SUM(tSPME) AS INT)/60)||':'||printf('%02d',CAST(SUM(tSPME) AS INT)%60) AS 'SP ME', "
            "printf('%02d',CAST(SUM(tNIGHT) AS INT)/60)||':'||printf('%02d',CAST(SUM(tNIGHT) AS INT)%60) AS 'NIGHT', "
            "printf('%02d',CAST(SUM(tIFR) AS INT)/60)||':'||printf('%02d',CAST(SUM(tIFR) AS INT)%60) AS 'IFR', "
            "printf('%02d',CAST(SUM(tPIC) AS INT)/60)||':'||printf('%02d',CAST(SUM(tPIC) AS INT)%60) AS 'PIC', "
            "printf('%02d',CAST(SUM(tPICUS) AS INT)/60)||':'||printf('%02d',CAST(SUM(tPICUS) AS INT)%60) AS 'PICUS', "
            "printf('%02d',CAST(SUM(tSIC) AS INT)/60)||':'||printf('%02d',CAST(SUM(tSIC) AS INT)%60) AS 'SIC', "
            "printf('%02d',CAST(SUM(tDual) AS INT)/60)||':'||printf('%02d',CAST(SUM(tDual) AS INT)%60) AS 'DUAL', "
            "printf('%02d',CAST(SUM(tFI) AS INT)/60)||':'||printf('%02d',CAST(SUM(tFI) AS INT)%60) AS 'INSTRUCTOR', "
            "printf('%02d',CAST(SUM(tSIM) AS INT)/60)||':'||printf('%02d',CAST(SUM(tSIM) AS INT)%60) AS 'SIMULATOR', "
            "printf('%02d',CAST(SUM(tMP) AS INT)/60)||':'||printf('%02d',CAST(SUM(tMP) AS INT)%60) AS 'MultPilot', "
            "CAST(SUM(toDay) AS INT) AS 'TO Day', CAST(SUM(toNight) AS INT) AS 'TO Night', "
            "CAST(SUM(ldgDay) AS INT) AS 'LDG Day', CAST(SUM(ldgNight) AS INT) AS 'LDG Night' "
            "FROM flights";
    QVector<QString> columns = {"total", "spse", "spme", "night", "ifr",
                                "pic", "picus", "sic", "dual", "fi", "sim", "multipilot",
                                "today", "tonight", "ldgday", "ldgnight"
                               };
    QSqlQuery q(statement);
    QVector<QPair<QString, QString>> output;
    QString value;
    q.next();
    for (const auto &column : columns) {
        value = q.value(columns.indexOf(column)).toString();
        if (!value.isEmpty()) {
            output << QPair{column, value};
        } else {
            output << QPair{column, QString("00:00")};
        }
    }
    return output;
}
