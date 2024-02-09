#include "time.h"
#include "math.h"

namespace OPL {

Time::Time(const DateTimeFormat &format)
    : m_format(format),
    m_minutes(-1)
{}

Time::Time(const QTime &qTime, const DateTimeFormat &format)
{
    m_minutes = qTime.isValid() ? qTime.minute() + qTime.hour() * 60 : -1;
}

Time::Time(int32_t minutes, const DateTimeFormat &format)
    : m_minutes(minutes), m_format(format)
{}

bool Time::isValidTimeOfDay() const
{
    return isValid() && m_minutes <= MINUTES_PER_DAY;
}

bool Time::isValid() const
{
    return m_minutes >= 0;
}

const QString Time::toString() const
{
    switch(m_format.timeFormat()) {
    case DateTimeFormat::TimeFormat::Default:
        return QString::number(m_minutes / 60).rightJustified(2, '0')
               + ':'
               + QString::number(m_minutes % 60).rightJustified(2, '0');
    case DateTimeFormat::TimeFormat::Decimal:
        return QString::number(m_minutes / 60.0, 'f', 2);
        break;
    case DateTimeFormat::TimeFormat::Custom:
        return QTime(0, m_minutes, 0).toString(m_format.timeFormatString());
    default:
        return QString();
    }
}

int32_t Time::toMinutes() const
{
    return m_minutes;
}

Time Time::fromString(const QString &timeString, const DateTimeFormat &format)
{
    switch(format.timeFormat()) {
    case DateTimeFormat::TimeFormat::Default:
    {
        const auto qTime = QTime::fromString(timeString, QStringLiteral("hh:mm"));
        return Time(qTime, format);
        break;
    }
    case DateTimeFormat::TimeFormat::Decimal:
    {
        // try to convert string to double
        bool ok = false;
        const double timeValue = timeString.toDouble(&ok);

        if(!ok) {
            return {-1, format};
        }

        // extract integer and fractional part
        double hours, minutes;
        hours = modf(timeValue, &minutes);

        // create the Time Object
        return(Time(hours * 60 + minutes, format));
        break;
    }
    case DateTimeFormat::TimeFormat::Custom:
        const auto qTime = QTime::fromString(timeString, format.timeFormatString());
        return Time(qTime, format);
        break;
    }
}

Time Time::blockTime(const Time &offBlocks, const Time &onBlocks)
{
    // make sure both times are in 24h range
    bool bothTimesAreValid = offBlocks.isValidTimeOfDay() && onBlocks.isValidTimeOfDay();
    if(!bothTimesAreValid)
        return {-1, offBlocks.m_format};

    // calculate the block time
    if(onBlocks.m_minutes > offBlocks.m_minutes) {
        // take-off and landing on the same day
        return Time(onBlocks.m_minutes - offBlocks.m_minutes, offBlocks.m_format);
    } else {
        if(offBlocks.m_minutes == onBlocks.m_minutes)
            return Time(0, offBlocks.m_format);
        // landing the day after take off
        int minutesToMidnight = MINUTES_PER_DAY - offBlocks.m_minutes;
        return Time(minutesToMidnight + onBlocks.m_minutes, offBlocks.m_format);
    }
}

int32_t Time::blockMinutes(const Time &offBlocks, const Time &onBlocks)
{
    return blockTime(offBlocks, onBlocks).toMinutes();
}

} // namespace OPL
