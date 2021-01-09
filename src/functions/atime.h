#ifndef ATIME_H
#define ATIME_H

#include <QtCore>
#include <QTime>
#include "src/oplconstants.h"
#include "src/testing/adebug.h"

namespace ATime {

/*!
 * \brief Converts a QTime to a String to be used in the UI
 */
inline const QString toString(const QTime &time, opl::time::FlightTimeFormat format = opl::time::Default)
{
    switch (format) {
    case opl::time::Default:
        return time.toString(QStringLiteral("hh:mm"));
        break;
    case opl::time::Decimal:
        return QString::asprintf("%.2f", (time.hour() * 60 + time.minute() )/60.0);
        break;
    default:
        return QString();
    }
}

/*!
 * \brief Converts an integer of minutes as received from the Datbase to a String
 */
inline const QString toString(int minutes_in, opl::time::FlightTimeFormat format = opl::time::Default)
{
    switch (format) {
    case opl::time::Default:
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
    case opl::time::Decimal:
    {
        int hour = minutes_in / 60;
        double minute = (minutes_in % 60) / 60.0;
        return QString::asprintf("%.2f", (hour+minute));
    }
    default:
        return QString();
    }

}

inline double toDecimalHours(const QTime &time){
    return (time.hour() * 60 + time.minute()) / 60.0;
}

inline int toMinutes(const QTime &time) {return time.hour() * 60 + time.minute();}

inline QTime fromMinutes(int total_minutes)
{
    int minute = total_minutes % 60;
    int hour = total_minutes / 60;

    return QTime(hour, minute, 0);
}

inline const QTime fromString(QString time_string, opl::time::FlightTimeFormat format = opl::time::Default)
{
    switch (format) {
    case opl::time::Default:
        return QTime::fromString(time_string, QStringLiteral("hh:mm"));
        break;
    case opl::time::Decimal:
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

inline const QTime fromString(const char* time_string, opl::time::FlightTimeFormat format = opl::time::Default)
{
    switch (format) {
    case opl::time::Default:
        return QTime::fromString(time_string, QStringLiteral("hh:mm"));
        break;
    case opl::time::Decimal:
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
