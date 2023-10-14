#include "time.h"

namespace OPL {

Time::Time()
{
}

bool Time::isValidTimeOfDay()
{
    return m_minutes <= MINUTES_PER_DAY;
}

const QString Time::toString() const
{
    // convert to hh:mm
    return QString::number(m_minutes / 60).rightJustified(2, '0') + QLatin1Char(':') + QString::number(m_minutes % 60).rightJustified(2, '0');
}

int32_t Time::toMinutes()
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

} // namespace OPL
