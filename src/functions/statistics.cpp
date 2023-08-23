/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
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
#include "statistics.h"
#include "src/database/database.h"

/*!
 * \brief OPL::Statistics::totalTime Looks up Total Blocktime in the flights database
 * \param TimeFrame - The timeframe used for the calculations.
 * \return Amount of Total Block Time in minutes
 */
int OPL::Statistics::totalTime(TimeFrame time_frame)
{
    QString statement;
    QDate start;
    QString start_date;

    switch (time_frame) {
    case TimeFrame::AllTime:
        statement = QStringLiteral("SELECT SUM(tblk) FROM flights");
        break;
    case TimeFrame::CalendarYear:
        start.setDate(QDate::currentDate().year(), 1, 1);
        start_date = start.toString(Qt::ISODate);
        start_date.append(QLatin1Char('\''));
        start_date.prepend(QLatin1Char('\''));
        statement = QLatin1String("SELECT SUM(tblk) FROM flights WHERE doft >= ") + start_date;
        break;
    case TimeFrame::Rolling12Months:
        start = QDate::fromJulianDay(QDate::currentDate().toJulianDay() - 365);
        start_date = start.toString(Qt::ISODate);
        start_date.append(QLatin1Char('\''));
        start_date.prepend(QLatin1Char('\''));
        statement = QLatin1String("SELECT SUM(tblk) FROM flights WHERE doft >= ") + start_date;
        break;
    case TimeFrame::Rolling28Days:
        start = QDate::fromJulianDay(QDate::currentDate().toJulianDay() - 28);
        start_date = start.toString(Qt::ISODate);
        start_date.append(QLatin1Char('\''));
        start_date.prepend(QLatin1Char('\''));
        statement = QLatin1String("SELECT SUM(tblk) FROM flights WHERE doft >= ") + start_date;
        break;
    }

    auto db_return = DB->customQuery(statement, 1);

    if (!db_return.isEmpty())
        return db_return.first().toInt();

    return 0;
}

/*!
 * \brief OPL::Statistics::currencyTakeOffLanding Returns the amount of Take Offs and
 * Landings performed in the last x days. If no vallue for days is provided, 90 is used,
 * as per EASA regulations
 * \return QVector<QString>{#TO,#LDG}
 */
QVector<QVariant> OPL::Statistics::countTakeOffLanding(int days)
{
    QDate start = QDate::fromJulianDay(QDate::currentDate().toJulianDay() - days);
    QString startdate = start.toString(Qt::ISODate);
    startdate.append(QLatin1Char('\''));
    startdate.prepend(QLatin1Char('\''));

    QString statement = QLatin1String("SELECT "
                                      " SUM(IFNULL(flights.toDay,0) + IFNULL(flights.toNight,0)) AS 'TO', "
                                      " SUM(IFNULL(flights.ldgDay,0) + IFNULL(flights.ldgNight,0)) AS 'LDG' "
                                      " FROM flights "
                                      " WHERE doft >=") + startdate;

    QVector<QVariant> result = DB->customQuery(statement, 2);
    // make sure a value is returned instead of NULL
    for (const auto &var : result) {
        if (var.isNull())
            result.replace(result.indexOf(var), 0);
    }

    return result;
}

QVector<QPair<QString, QString>> OPL::Statistics::totals()
{
    QString statement = QStringLiteral("SELECT "
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
            "FROM flights");
    QVector<QString> columns = {QLatin1String("total"), QLatin1String("spse"), QLatin1String("spme"),
                                QLatin1String("night"), QLatin1String("ifr"),  QLatin1String("pic"),
                                QLatin1String("picus"), QLatin1String("sic"),  QLatin1String("dual"),
                                QLatin1String("fi"),    QLatin1String("sim"),  QLatin1String("multipilot"),
                                QLatin1String("today"), QLatin1String("tonight"), QLatin1String("ldgday"),
                                QLatin1String("ldgnight")
                               };
    QSqlQuery query(statement);
    QVector<QPair<QString, QString>> output;
    QString value;
    query.next();
    for (const auto &column : columns) {
        value = query.value(columns.indexOf(column)).toString();
        if (!value.isEmpty()) {
            output.append(QPair<QString, QString>{column, value});
        } else {
            output.append(QPair<QString, QString>{column, QLatin1String("00:00")});
        }
    }
    return output;
}
/*!
 * \brief Calculates the date of expiry for the take-off and landing currency.
 *
 * The default value for days is 90.
 * \return
 */
QDate OPL::Statistics::currencyTakeOffLandingExpiry(int expiration_days)
{
    int number_of_days = 0;
    QVector<QVariant> takeoff_landings;

    // Check if enough take-offs and landings exist within the expiration period, if that's not the case
    // we are out of currency and we can stop right there.
    takeoff_landings = countTakeOffLanding(expiration_days);
    if (takeoff_landings[0].toInt() < 3 || takeoff_landings[1].toInt() < 3)
        return QDate::currentDate();

    // Go back in time to find a point at which number of Take-Offs and Landings >= 3
    for (int i=0; i <= expiration_days; i++) {
        takeoff_landings = countTakeOffLanding(i);
        //DEB << takeoff_landings;
        if (takeoff_landings[0].toInt() >= 3 && takeoff_landings[1].toInt() >= 3) {
            number_of_days = i;
            //DEB << "Loop position i =" << i;
            break;
        }
    }
    // The expiration date of currency is now currentDate - number of days + expiration_days (default 90)
    QDate expiration_date = QDate::fromJulianDay(QDate::currentDate().toJulianDay() - number_of_days);
    //DEB << expiration_date.addDays(expiration_days);

    return expiration_date.addDays(expiration_days);;
}

