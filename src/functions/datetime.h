#ifndef DATETIME_H
#define DATETIME_H
#include "src/opl.h"

namespace OPL {

class DateTime {
public:
    const inline static QString ISO_FORMAT_STRING = QStringLiteral("yyyy-MM-dd");
    const inline static QString DE_FORMAT_STRING = QStringLiteral("dd.MM.yyyy");
    const inline static QString EN_FORMAT_STRING = QStringLiteral("MM/dd/yyyy");

    const static inline QMap<OPL::DateFormat, QString> DATEFORMATSMAP = {
        {OPL::DateFormat::ISODate, ISO_FORMAT_STRING},
        {OPL::DateFormat::DE,      DE_FORMAT_STRING },
        {OPL::DateFormat::EN,      EN_FORMAT_STRING },

    };

    const static inline QStringList DISPLAY_NAMES = {
        QStringLiteral("ISO 8601: yyyy-MM-dd"),
        QStringLiteral("DE: dd.MM.yyyy"),
        QStringLiteral("EN: MM/dd/yyyy")
    };

    /*!
     * \brief Reimplements QDate::toString to accept OPL::Date::DateFormat enums
     */
    inline static QString dateToString(const QDate &date, OPL::DateFormat format = OPL::DateFormat::ISODate)
    {
        return date.toString(DATEFORMATSMAP.value(format));
    };

    /*!
     * \brief takes a user-provided input and tries to convert it to a (valid) QDate.
     * \return QDate (invalid if input not recognized)
     */
    static QDate parseInput(QString &user_input, OPL::DateFormat format);

    static void tryToFix(QString &user_input, OPL::DateFormat format);

    /*!
     * \brief padCentury adds the century to a date where it was omitted
     */
    static void padCentury(QString &io_user_input, OPL::DateFormat format);

    /*!
     * \brief pads a user-provided date string with 0s to facilitate conversion to QDate
     */
    static void padZeroes(QString &io_user_input);

    static void addSeperators(QString &io_user_input, const OPL::DateFormat &format);

    static bool containsSeperator(const QString &user_input);



    static const QStringList& getDisplayNames();

    static const QString getFormatString(OPL::DateFormat format);

    /*!
     * \brief today Returns a string containing the current date in ISO format
     * \return
     */
    static const QString currentDate();

    /*!
     * \brief dateTimeToString formats a QDateTime object into a string in a uniform way.
     * \return
     */
    static inline const QString dateTimeToString (const QDateTime& date_time, OPL::DateTimeFormat format) {
        switch (format) {
        case OPL::DateTimeFormat::Default:
            return date_time.toString(Qt::ISODate);
        case OPL::DateTimeFormat::Backup:
            return date_time.toString(QStringLiteral("yyyy_MM_dd_T_hh_mm"));
        default:
            return QString();
        }
    }

    static inline QDateTime fromString(const QString& date_time_string)
    {
        auto date_time = QDateTime::fromString(date_time_string, QStringLiteral("yyyy-MM-ddhh:mm"));
        date_time.setTimeZone(QTimeZone::utc());
        return date_time;
    }

};



/*!
 * \brief The ADate class is responsible for input/output of Dates and handling the different
 * Date Formats.
 */
//class ADate
//{
//public:
//    /*!
//     * \brief takes a user-provided input and tries to convert it to a (valid) QDate.
//     * \return QDate (invalid if input not recognized)
//     */
//    static QDate parseInput(QString &iuser_input, OPL::DateFormat format);
//
//    static void tryToFix(QString &user_input, OPL::DateFormat format);
//
//    /*!
//     * \brief padCentury adds the century to a date where it was omitted
//     */
//    static void padCentury(QString &io_user_input, OPL::DateFormat format);
//
//    /*!
//     * \brief pads a user-provided date string with 0s to facilitate conversion to QDate
//     */
//    static void padZeroes(QString &io_user_input);
//
//    static void addSeperators(QString &io_user_input, const OPL::DateFormat &format);
//
//    static bool containsSeperator(const QString &user_input);
//
//
//
//    static const QStringList& getDisplayNames();
//
//    static const QString getFormatString(OPL::DateFormat format);
//
//    /*!
//     * \brief today Returns a string containing the current date in ISO format
//     * \return
//     */
//    static const QString currentDate();
//
//};

} // namespace OPL

#endif // DATETIME_H
