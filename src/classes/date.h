#ifndef DATE_H
#define DATE_H
#include <QDate>
#include <QLocale>
namespace OPL {

/*!
 * \brief The Date class wraps the QDate class.
 * \details The QDate class stores dates internally as a Julian Day number,
 * an integer count of every day in a contiguous range, with 24 November 4714 BCE
 * in the Gregorian calendar being Julian Day 0 (1 January 4713 BCE in the Julian calendar).
 *
 * Storing a given date as an integer value allows for easy conversion to localised strings
 * as well as calculations like date ranges.
 */
class Date
{
public:

    /*!
     * \brief Enumerates how dates can be formatted to a localised format.
     * \value Default - The Application standard, Equivalent to Qt::ISODate
     * \value SystemLocaleLong - The current system locale, elaborate
     * \value SystemLocaleShort - The current system locale, short
     */
    enum Format {Default, SystemLocaleLong, SystemLocaleShort};

    Date() = delete;

    Date(int julianDay);

    const static inline Date fromString(const QString &dateString, Format format = Default) {
        switch (format) {
        case Default:
            return Date(QDate::fromString(dateString, Qt::ISODate).toJulianDay());
        case SystemLocaleLong:
            return Date(QDate::fromString(dateString, QLocale::system().dateFormat(QLocale::LongFormat)).toJulianDay());
        case SystemLocaleShort:
            return Date(QDate::fromString(dateString, QLocale::system().dateFormat(QLocale::ShortFormat)).toJulianDay());
        default:
            return Date(0);
            break;
        }
    }

    const static inline Date fromJulianDay(int julianDay) { return Date(julianDay); }

    const static inline Date today() { return Date(QDate::currentDate().toJulianDay()); }

    const static inline QString julianDayToString(int julianDay) { return Date(julianDay).toString(); }

    const QString toString(Format format = Format::Default) const;

    const bool isValid() { return date.isValid(); }

private:
    QDate date;

};

} // namespace OPL

#endif // DATE_H
