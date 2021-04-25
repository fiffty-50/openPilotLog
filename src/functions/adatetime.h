/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#ifndef ADATETIME_H
#define ADATETIME_H
#include <QtCore>
#include "src/opl.h"

namespace ADateTime {

/*!
 * \brief toString formats a QDateTime object into a string in a uniform way.
 * \return
 */
inline const QString toString (const QDateTime date_time, Opl::Datetime::DateTimeFormat format) {
    switch (format) {
    case Opl::Datetime::Default:
        return date_time.toString(Qt::ISODate);
    case Opl::Datetime::Backup:
        return date_time.toString(QStringLiteral("yyyy_MM_dd_T_hh_mm"));
    default:
        return QString();
    }
}

}

#endif // ADATETIME_H
