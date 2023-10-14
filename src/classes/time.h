#ifndef TIME_H
#define TIME_H

#include <QtCore>
namespace OPL {

/**
 * \brief The Time class handles conversions between user input / user-facing time data display and
 * database format.
 * \details Time data in the database is stored as an integer value of minutes, whereas the user-facing
 * time data is normally displayed in the Qt::ISODATE format. A database value of 72 would for example be
 * displayed as 01:12 (1 hour and 12 minutes).
 */
class Time
{
private:
    const static inline int MINUTES_PER_DAY = 24 * 60;
    int32_t m_minutes = 0;

public:
    Time();
    Time(int32_t minutes) : m_minutes(minutes) {};

    /**
     * @brief isValidTimeOfDay - determines whether the instance can be converted to a time hh:mm
     * @return true if the total amount of minutes does not exceed one day.
     */
    bool isValidTimeOfDay();

    /**
     * @brief toString returns the time as hh:mm
     */
    const QString toString() const;

    /**
     * @brief toMinutes - returns the number of minutes in the time Object
     */
    int32_t toMinutes();

    /**
     * @brief fromString create a Time Object from a String formatted as hh:mm
     * @param timeString the input string
     * @return the Time Object corresponding to the string, equivalent to 0 minutes if conversion fails.
     */
    static Time fromString(const QString& timeString);
};

}// namespace OPL

#endif // TIME_H
