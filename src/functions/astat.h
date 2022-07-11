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
#ifndef ASTAT_H
#define ASTAT_H
#include <QtCore>
#include <QSqlQuery>
#include <QSqlError>

namespace AStat {

/*!
 * \brief The AStat namespace provides functionality for retreiving various statistics
 * from the database, such as total times or recency. In general, most values are
 * provided as either QString or QVector<QString>.
 */


    enum class TimeFrame {AllTime, CalendarYear, Rolling12Months, Rolling28Days};

    enum class ToLdg {Takeoff, Landing};

    int totalTime(TimeFrame time_frame);

    QVector<QVariant> countTakeOffLanding(int days = 90);

    QDate currencyTakeOffLandingExpiry(int expiration_days = 90);

    QVector<QPair<QString, QString>> totals();

} // namespace AStat

#endif // ASTAT_H
