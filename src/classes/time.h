#ifndef TIME_H
#define TIME_H

#include "src/opl.h"
#include <QtCore>
namespace OPL {

/**
 * \brief The Time class handles conversions between user input / user-facing time data display and
 * database format.
 * \details Time data in the database is stored as an integer value of minutes, whereas the user-facing
 * time data is normally displayed in accordance with the selected DateTimeFormat, by default "hh:mm".
 */
class Time
{
public:
    Time() = delete;
    Time(const DateTimeFormat &format);
    Time(const QTime &qTime, const DateTimeFormat &format);
    Time(int32_t minutes, const DateTimeFormat &format);;

    enum TimeFrame {Day, Week, Year};

    /**
     * @brief isValidTimeOfDay - determines whether the instance can be converted to a time hh:mm
     * @return true if the total amount of minutes does not exceed one day.
     */
    bool isValidTimeOfDay() const;

    /*!
     * \brief a time is considered valid if it has a time value of >= 0
     */
    bool isValid() const;

    /**
     * @brief toString returns the time as hh:mm
     */
    const QString toString() const;

    /**
     * @brief toMinutes - returns the number of minutes in the time Object
     */
    int32_t toMinutes() const;

    /**
     * @brief fromString create a Time Object from a String formatted as hh:mm
     * @param timeString the input string
     * @return the Time Object corresponding to the string, equivalent to 0 minutes if conversion fails.
     */
    static Time fromString(const QString& timeString, const DateTimeFormat &format);

    /*!
     * \brief Calculate the elapsed time between two events
     * \param offBlocks - The start tmie
     * \param onBlocks - the end time
     * \return The elapsed time
     */
    static Time blockTime(const Time &offBlocks, const Time& onBlocks);

    /*!
     * \brief Calculate elapsed time between two events
     * \param offBlocks - The start time
     * \param onBlocks - The end time
     * \return the elapsed time in minutes
     */
    static int32_t blockMinutes(const Time &offBlocks, const Time& onBlocks);

    /*!
     * \brief toMinutes returns the number of minutes in the given time frame
     * \param count - The number of time frames (e.g. '7' days)
     * \return
     */
    static constexpr int timeFrameToMinutes(TimeFrame timeFrame, int count) {
        switch (timeFrame) {
        case Day:
            return count * MINUTES_PER_DAY;
        case Week:
            return count * 7 * MINUTES_PER_DAY;
        case Year:
            return count * 7 * 52 * MINUTES_PER_DAY;
        default:
            return 0;
        }
    }


private:
    static constexpr int MINUTES_PER_DAY = 24 * 60;
    static constexpr QLatin1Char DECIMAL_SEPERATOR = QLatin1Char('.');

    DateTimeFormat m_format;
    int32_t m_minutes;

};

}// namespace OPL

#endif // TIME_H
