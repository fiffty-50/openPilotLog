#include "adate.h"

QDate ADate::parseInput(QString &io_user_input, Opl::Date::ADateFormat format)
{
    // Try input string
    const QString &format_string = ADATEFORMATSMAP.value(format);
    QDate return_date = QDate::fromString(io_user_input, format_string);
    if (return_date.isValid())
        return return_date;

    // try to fix the user input
    tryToFix(io_user_input, format);

    return  QDate::fromString(io_user_input, format_string);
}

void ADate::tryToFix(QString &io_user_input, Opl::Date::ADateFormat format)
{

    if (io_user_input.length() < 10) {
        if (containsSeperator(io_user_input)) {
            padZeroes(io_user_input);
        }
        else {
            addSeperators(io_user_input, format);
            padZeroes(io_user_input);
        }
    }

    if (io_user_input.length() == 8)
        padCentury(io_user_input, format);

}

// Input contains seperators and is of length 8
void ADate::padCentury(QString &io_user_input, Opl::Date::ADateFormat format)
{
    switch (format) {
    case Opl::Date::ADateFormat::ISODate: {
        int year = io_user_input.leftRef(2).toInt();
        if (year > 50)
            io_user_input.prepend(QStringLiteral("19"));
        else
            io_user_input.prepend(QStringLiteral("20"));
        break;
    }
    case Opl::Date::ADateFormat::DE: {
        int year = io_user_input.rightRef(2).toInt();
        if (year > 50)
            io_user_input.insert(6, QStringLiteral("19"));
        else
            io_user_input.insert(6, QStringLiteral("20"));
        break;
    }
    case Opl::Date::ADateFormat::EN: {
        int year = io_user_input.rightRef(2).toInt();
        if (year > 50)
            io_user_input.insert(6, QStringLiteral("19"));
        else
            io_user_input.insert(6, QStringLiteral("20"));
        break;
    }
    }
    DEB << "Padded century: " << io_user_input;
}

void ADate::padZeroes(QString &io_user_input)
{
    const auto unpadded_start = QRegularExpression(QStringLiteral("^\\d{1}\\W"));
    const auto unpadded_middle = QRegularExpression(QStringLiteral("\\W\\d\\W"));
    const auto unpadded_end = QRegularExpression(QStringLiteral("\\W\\d$"));

    auto match = unpadded_start.match(io_user_input);
    if (match.hasMatch())
        io_user_input.insert(match.capturedStart(), QLatin1Char('0'));

    match = unpadded_middle.match(io_user_input);
    if (match.hasMatch())
        io_user_input.insert(match.capturedStart() + 1, QLatin1Char('0'));

    match = unpadded_end.match(io_user_input);
    if (match.hasMatch())
        io_user_input.insert(match.capturedStart() + 1, QLatin1Char('0'));
    DEB << "Padded zeroes: " << io_user_input;
}
// 10.10.2020
void ADate::addSeperators(QString &io_user_input, const Opl::Date::ADateFormat &format)
{
    switch (format) {
    case Opl::Date::ADateFormat::ISODate:
        if (io_user_input.length() > 7) {
            io_user_input.insert(4, QLatin1Char('-'));
            io_user_input.insert(7, QLatin1Char('-'));
        } else {
            io_user_input.insert(2, QLatin1Char('-'));
            io_user_input.insert(5, QLatin1Char('-'));
        }
        break;
    case Opl::Date::ADateFormat::DE:
            io_user_input.insert(2, QLatin1Char('.'));
            io_user_input.insert(5, QLatin1Char('.'));
        break;
    case Opl::Date::ADateFormat::EN:
            io_user_input.insert(2, QLatin1Char('/'));
            io_user_input.insert(5, QLatin1Char('/'));
        break;
    }
    DEB << "Added Seperators: " << io_user_input;
}

bool ADate::containsSeperator(const QString &user_input)
{
    if (user_input.contains(QLatin1Char('.')))
        return true;
    if (user_input.contains(QLatin1Char('-')))
        return true;
    if (user_input.contains(QLatin1Char('/')))
        return true;

    DEB << "No Seperators found.";

    return false;
}

const QStringList& ADate::getDisplayNames()
{
    return DISPLAY_NAMES;
}

const QString ADate::getFormatString(Opl::Date::ADateFormat format)
{
    return ADATEFORMATSMAP.value(format);
}

