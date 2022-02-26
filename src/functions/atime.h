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
#ifndef ATIME_H
#define ATIME_H

#include <QtCore>
#include <QTime>
#include "src/opl.h"
#include "src/functions/alog.h"

namespace ATime {

/*!
 * \brief Converts a QTime to a String to be used in the UI
 */
inline const QString toString(const QTime &time, Opl::Time::FlightTimeFormat format = Opl::Time::Default)
{
    switch (format) {
    case Opl::Time::Default:
        return time.toString(QStringLiteral("hh:mm"));
        break;
    case Opl::Time::Decimal:
        return QString::number(((time.hour() * 60 + time.minute() )/60.0), 'f', 2);
        break;
    default:
        return QString();
    }
}

/*!
 * \brief Converts an integer of minutes as received from the Datbase to a String
 */
inline const QString toString(int minutes_in, Opl::Time::FlightTimeFormat format = Opl::Time::Default)
{
    switch (format) {
    case Opl::Time::Default:
    {
        QString hour = QString::number(minutes_in / 60);
        if (hour.size() < 2) {
            hour.prepend(QStringLiteral("0"));
        }
        QString minute = QString::number(minutes_in % 60);
        if (minute.size() < 2) {
            minute.prepend(QStringLiteral("0"));
        }
        return hour + ':' + minute;
    }
    case Opl::Time::Decimal:
    {
        int hour = minutes_in / 60;
        double minute = (minutes_in % 60) / 60.0;
        return QString::number((hour + minute), 'f', 2);
    }
    default:
        return QString();
    }

}

inline double toDecimalHours(const QTime &time){
    return (time.hour() * 60 + time.minute()) / 60.0;
}

inline QTime fromMinutes(int total_minutes)
{
    int minute = total_minutes % 60;
    int hour = total_minutes / 60;

    return QTime(hour, minute, 0);
}

inline const QTime fromString(QString time_string, Opl::Time::FlightTimeFormat format = Opl::Time::Default)
{
    switch (format) {
    case Opl::Time::Default:
        return QTime::fromString(time_string, QStringLiteral("hh:mm"));
        break;
    case Opl::Time::Decimal:
    {
        double decimal_time = time_string.toDouble();
        int hour = decimal_time;
        int minute = round((decimal_time - hour) * 60);
        return QTime(hour, minute, 0);
        break;
    }
    default:
        return QTime();
    }
}

inline const QTime fromString(const char* time_string, Opl::Time::FlightTimeFormat format = Opl::Time::Default)
{
    switch (format) {
    case Opl::Time::Default:
        return QTime::fromString(time_string, QStringLiteral("hh:mm"));
        break;
    case Opl::Time::Decimal:
    {
        double decimal_time = QString(time_string).toDouble();
        int hour = decimal_time;
        int minute = round((decimal_time - hour) * 60);
        return QTime(hour, minute, 0);
        break;
    }
    default:
        return QTime();
    }
}

inline int toMinutes(const QTime &time) {return time.hour() * 60 + time.minute();}
inline int toMinutes(const QString &time_string) {return toMinutes(fromString(time_string));}

inline QTime blocktime(const QTime &tofb, const QTime &tonb)
{
    QTime blocktime_out(0, 0); // initialise return value at midnight

    if (tonb > tofb) { // landing same day
        int block_seconds = tofb.secsTo(tonb);
        blocktime_out = blocktime_out.addSecs(block_seconds);
    } else { // landing next day
        QTime midnight(0, 0);
        int seconds = tofb.secsTo(midnight);
        blocktime_out = blocktime_out.addSecs(seconds);
        seconds = midnight.secsTo(tonb);
        blocktime_out = blocktime_out.addSecs(seconds);
    }
    return blocktime_out;
}

inline QTime blocktime(const QString& tofb, const QString& tonb)
{
    QTime t_tofb = ATime::fromString(tofb);
    QTime t_tonb = ATime::fromString(tonb);
    return blocktime(t_tofb, t_tonb);
}

/*!
 * \brief blockMinutes calculates the total amount of minutes elapsed between
 * tofb and tonb
 */
inline int blockMinutes(const QString& tofb, const QString& tonb)
{
    const QTime t_tofb = ATime::fromString(tofb);
    const QTime t_tonb = ATime::fromString(tonb);
    if (t_tofb.isValid() && t_tonb.isValid()) {
        const auto tblk = ATime::blocktime(t_tofb, t_tonb);
        return ATime::toMinutes(tblk);
    } else
        return 0;
}

/*!
 * \brief blockMinutes calculates the total amount of minutes elapsed between
 * tofb and tonb
 */
inline int blockMinutes(const QTime& tofb, const QTime& tonb)
{
    if (tofb.isValid() && tonb.isValid()) {
        const auto tblk = ATime::blocktime(tofb, tonb);
        return ATime::toMinutes(tblk);
    } else
        return 0;
}

/*!
 * \brief verifies user input and formats to hh:mm
 * if the output is not a valid time, an empty string is returned. Accepts
 * input as hh:mm, h:mm, hhmm or hmm.
 * \param userinput from a QLineEdit
 * \return formatted QString "hh:mm" or Empty String
 */
inline const QString formatTimeInput(QString user_input)
{
    QTime temp_time; //empty time object is invalid by default

    bool contains_seperator = user_input.contains(':');
    if (user_input.length() == 4 && !contains_seperator) {
        temp_time = QTime::fromString(user_input, QStringLiteral("hhmm"));
    } else if (user_input.length() == 3 && !contains_seperator) {
        if (user_input.toInt() < 240) { //Qtime is invalid if time is between 000 and 240 for this case
            QString tempstring = user_input.prepend(QStringLiteral("0"));
            temp_time = QTime::fromString(tempstring, QStringLiteral("hhmm"));
        } else {
            temp_time = QTime::fromString(user_input, QStringLiteral("Hmm"));
        }
    } else if (user_input.length() == 4 && contains_seperator) {
        temp_time = QTime::fromString(user_input, QStringLiteral("h:mm"));
    } else if (user_input.length() == 5 && contains_seperator) {
        temp_time = QTime::fromString(user_input, QStringLiteral("hh:mm"));
    }

    auto output = temp_time.toString(QStringLiteral("hh:mm"));

    if (output.isEmpty()) {
        DEB << "Time input is invalid.";
    }
    return output;
}

} // namespace ATime

#endif // ATIME_H
