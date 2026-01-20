#ifndef DATE_H
#define DATE_H
#include "src/opl.h"
#include <QDate>
namespace OPL {

/*!
 * \brief The Date namespace holds Date constans and static functions
 * \details The QDate class stores dates internally as a Julian Day number,
 * an integer count of every day in a contiguous range, with 24 November 4714 BCE
 * in the Gregorian calendar being Julian Day 0 (1 January 4713 BCE in the Julian calendar).
 *
 * Storing a given date as an integer value allows for easy conversion to localised strings
 * as well as calculations like date ranges.
 *
 * Julian day is also used to store a date in the database.
 *
 * The default start date for entries in the application and database is the start of the
 * 20th century (1900-01-01) and the highest possible date value is 9999-12-12.
 */
namespace Date {
static constexpr int JULIAN_DAY_START = 2415021; // 1900-01-01
static constexpr int JULIAN_DAY_END   = 5373465; // 9999-12-12

/*!
 * \brief returns the julian day of 1900-01-01
 */
constexpr static int minimumDateJd() { return JULIAN_DAY_START; }

/*!
 * \brief returns the QDate for 1900-01-01
 */
const static inline QDate minimumDate() { return QDate::fromJulianDay(JULIAN_DAY_START); }

/*!
 * \brief returns the julian day of 9999-12-12
 */
constexpr static int maximumDateJd() { return JULIAN_DAY_END; }

/*!
 * \brief returns the QDate for 9999-12-12
 */
const static inline QDate maximumDate() { return QDate::fromJulianDay(JULIAN_DAY_END); }

/*!
 * \brief Determine whether an integer is a valid julian Day within the accepted range
 */
constexpr static bool julianDayIsValid(int jd)
{
    return jd <= JULIAN_DAY_END && jd >= JULIAN_DAY_START;
}
} // namespace Date

} // namespace OPL

#endif // DATE_H
