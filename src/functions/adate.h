#ifndef ADATE_H
#define ADATE_H
#include "src/opl.h"

const static auto ISO = QStringLiteral("yyyy-MM-dd");
const static auto DE = QStringLiteral("dd.MM.yyyy");
const static auto EN = QStringLiteral("MM/dd/yyyy");

const static QMap<Opl::Date::ADateFormat, QString> ADATEFORMATSMAP = {
    {Opl::Date::ADateFormat::ISODate, ISO},
    {Opl::Date::ADateFormat::DE,      DE },
    {Opl::Date::ADateFormat::EN,      EN },

};

const static QStringList DISPLAY_NAMES = {
    QStringLiteral("ISO 8601: yyyy-MM-dd"),
    QStringLiteral("DE: dd.MM.yyyy"),
    QStringLiteral("EN: MM/dd/yyyy")
};

/*!
 * \brief The ADate class is responsible for input/output of Dates and handling the different
 * Date Formats.
 */
class ADate
{
public:
    /*!
     * \brief formatInput takes a user-provided input and tries to convert it to a QDate.
     * \return QDate (invalid if input not recognized)
     */
    static QDate formatInput(QString user_input, Opl::Date::ADateFormat format);

    /*!
     * \brief Reimplements QDate::toString to accept Opl::Date::ADateFormat enums
     */
    inline static QString toString(const QDate &date, Opl::Date::ADateFormat format = Opl::Date::ADateFormat::ISODate)
    {
        return date.toString(ADATEFORMATSMAP.value(format));
    };

    static const QStringList& getDisplayNames();

    static const QString getFormatString(Opl::Date::ADateFormat format);

};

#endif // ADATE_H
