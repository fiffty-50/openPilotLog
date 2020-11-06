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
#ifndef STAT_H
#define STAT_H

#include "src/database/db.h"
#include <QDateTime>

/*!
 * \brief The stat class provides functionality for retreiving various statistics
 * from the database, such as total times or recency. In general, most values are
 * provided as either QString or QVector<QString>.
 */
class Stat
{
public:

    enum yearType {allYears, calendarYear, rollingYear};

    static QString totalTime(yearType);

    static QVector<QString> currencyTakeOffLanding(int days);

    static QVector<QPair<QString, QString>> totals();

};

#endif // STAT_H
