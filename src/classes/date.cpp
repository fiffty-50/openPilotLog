#include "date.h"

namespace OPL {

Date::Date(int julianDay)
{
    date = QDate::fromJulianDay(julianDay);
}

const QString Date::toString(Format format) const
{
    switch (format) {
    case Default:
        return date.toString(Qt::ISODate);
    case SystemLocaleLong:
        return date.toString(QLocale::system().dateFormat(QLocale::LongFormat));
    case SystemLocaleShort:
        return date.toString(QLocale::system().dateFormat(QLocale::ShortFormat));
    default:
        return QString();
    }
}


} // namespace OPL
