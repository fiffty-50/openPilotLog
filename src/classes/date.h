#ifndef DATE_H
#define DATE_H
#include <QDate>
#include "src/opl.h"
namespace OPL {

/*!
 * \brief The Date class wraps the QDate class.
 * \details The QDate class stores dates internally as a Julian Day number,
 * an integer count of every day in a contiguous range, with 24 November 4714 BCE
 * in the Gregorian calendar being Julian Day 0 (1 January 4713 BCE in the Julian calendar).
 *
 * Storing a given date as an integer value allows for easy conversion to localised strings
 * as well as calculations like date ranges.
 *
 * Julian day is also used to store a date in the database.
 */
class Date
{
public:

    Date() = delete;
    Date(int julianDay, const DateTimeFormat &format);
    Date(const QString &textDate, const DateTimeFormat &format);
    Date(const QDate &date, const DateTimeFormat &format);

    const QString toString() const;
    const bool isValid() const { return m_date.isValid(); }

    const inline int toJulianDay() const { return m_date.toJulianDay(); }
    const static inline Date today(const DateTimeFormat &format) { return Date(QDate::currentDate().toJulianDay(), format); }

//    void setDateFormat(const DateFormat_ &format) {m_format = format}
    // todo copy constructor

private:
    QDate m_date;
    DateTimeFormat m_format;
};



} // namespace OPL

#endif // DATE_H
