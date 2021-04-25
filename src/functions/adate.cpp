#include "adate.h"

QDate ADate::formatInput(QString user_input, Opl::Date::ADateFormat format)
{
    const QString &format_string = ADATEFORMATSMAP.value(format);
    QDate return_date = QDate::fromString(user_input, format_string);
    if (return_date.isValid())
        return return_date;

     // If not successfull, try to fix the user input, it is allowable to ommit date seperators
    switch (format) {
    case Opl::Date::ADateFormat::ISODate:
        user_input.insert(4, QLatin1Char('-'));
        user_input.insert(7, QLatin1Char('-'));
        break;
    case Opl::Date::ADateFormat::DE:
        user_input.insert(2, QLatin1Char('.'));
        user_input.insert(5, QLatin1Char('.'));
        break;
    case Opl::Date::ADateFormat::EN:
        user_input.insert(2, QLatin1Char('/'));
        user_input.insert(5, QLatin1Char('/'));
        break;
    }

    return  QDate::fromString(user_input, format_string);
}

const QStringList& ADate::getDisplayNames()
{
    return DISPLAY_NAMES;
}

const QString ADate::getFormatString(Opl::Date::ADateFormat format)
{
    return ADATEFORMATSMAP.value(format);
}

