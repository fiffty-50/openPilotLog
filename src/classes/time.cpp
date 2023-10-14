#include "time.h"

namespace OPL {

Time::Time()
{
}

bool Time::isValidTimeOfDay() const
{
    return m_minutes <= MINUTES_PER_DAY;
}

const QString Time::toString() const
{
    // convert to hh:mm
    return QString::number(m_minutes / 60).rightJustified(2, '0') + QLatin1Char(':') + QString::number(m_minutes % 60).rightJustified(2, '0');
}

int32_t Time::toMinutes() const
{
    return m_minutes;
}

Time Time::fromString(const QString &timeString)
{
    const QStringList parts = timeString.split(QChar(':'));
    if(parts.size() < 2)
        return {};


    int32_t hours = parts[0].toInt();
    int32_t minutes = parts[1].toInt();

    if(hours < 0 || minutes < 0)
        return{};

    return Time(hours * 60 + minutes);
}

Time Time::blockTime(const Time &offBlocks, const Time &onBlocks)
{
    // make sure both times are in 24h range
    bool bothTimesAreValid = offBlocks.isValidTimeOfDay() && onBlocks.isValidTimeOfDay();
    if(!bothTimesAreValid)
        return {};

    // calculate the block time
    if(onBlocks.m_minutes > offBlocks.m_minutes) {
        // take-off and landing on the same day
        return Time(onBlocks.m_minutes - offBlocks.m_minutes);
    } else {
        // landing the day after take off
        int minutesToMidnight = MINUTES_PER_DAY - offBlocks.m_minutes;
        return Time(minutesToMidnight + onBlocks.m_minutes);
    }
}

int32_t Time::blockMinutes(const Time &offBlocks, const Time &onBlocks)
{
    return blockTime(offBlocks, onBlocks).toMinutes();
}

} // namespace OPL
