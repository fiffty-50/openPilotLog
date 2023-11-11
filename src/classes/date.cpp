#include "date.h"
#include <QLocale>

namespace OPL {

Date::Date(int julianDay, const DateTimeFormat &format)
    : m_format(format)
{
    m_date = QDate::fromJulianDay(julianDay);
}

Date::Date(const QString &textDate, const DateTimeFormat &format)
    : m_format(format)
{
    switch(format.dateFormat()) {
    case DateTimeFormat::DateFormat::Default:
        m_date = QDate::fromString(textDate, Qt::ISODate);
        break;
    case DateTimeFormat::DateFormat::Custom:
        m_date = QDate::fromString(textDate, format.dateFormatString());
        break;
    case DateTimeFormat::DateFormat::SystemLocale:
        m_date = QDate::fromString(QLocale::system().dateFormat(QLocale::ShortFormat));
        break;
    default:
        break;
    }
}

Date::Date(const QDate &date, const DateTimeFormat &format)
    : m_format(format), m_date(date)
{}

const QString Date::toString() const
{
    switch (m_format.dateFormat()) {
    case DateTimeFormat::DateFormat::Default:
        return m_date.toString(Qt::ISODate);
    case DateTimeFormat::DateFormat::SystemLocale:
        return m_date.toString(QLocale::system().dateFormat(QLocale::ShortFormat));
    case DateTimeFormat::DateFormat::Custom:
        return m_date.toString(m_format.dateFormatString());
    default:
        return QString();
    }
}


} // namespace OPL
