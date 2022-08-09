#include "datetime.h"

namespace OPL {

QDate DateTime::parseInput(QString &user_input, OPL::DateFormat format)
{
    // Try input string
    const QString &format_string = DATEFORMATSMAP.value(format);
    QDate return_date = QDate::fromString(user_input, format_string);
    if (return_date.isValid())
        return return_date;

    // try to fix the user input
    tryToFix(user_input, format);

    return  QDate::fromString(user_input, format_string);
}

void DateTime::tryToFix(QString &user_input, OPL::DateFormat format)
{

    if (user_input.length() < 10) {
        if (containsSeperator(user_input)) {
            padZeroes(user_input);
        }
        else {
            addSeperators(user_input, format);
            padZeroes(user_input);
        }
    }

    if (user_input.length() == 8)
        padCentury(user_input, format);

}

// Input contains seperators and is of length 8
void DateTime::padCentury(QString &user_input, OPL::DateFormat format)
{
    switch (format) {
    case OPL::DateFormat::ISODate: {
        int year = user_input.left(2).toInt();
        if (year > 50)
            user_input.prepend(QStringLiteral("19"));
        else
            user_input.prepend(QStringLiteral("20"));
        break;
    }
    case OPL::DateFormat::DE: {
        int year = user_input.right(2).toInt();
        if (year > 50)
            user_input.insert(6, QStringLiteral("19"));
        else
            user_input.insert(6, QStringLiteral("20"));
        break;
    }
    case OPL::DateFormat::EN: {
        int year = user_input.right(2).toInt();
        if (year > 50)
            user_input.insert(6, QStringLiteral("19"));
        else
            user_input.insert(6, QStringLiteral("20"));
        break;
    }
    }
    DEB << "Padded century: " << user_input;
}

void DateTime::padZeroes(QString &user_input)
{
    const static auto unpadded_start = QRegularExpression(QStringLiteral("^\\d{1}\\W"));
    const static auto unpadded_middle = QRegularExpression(QStringLiteral("\\W\\d\\W"));
    const static auto unpadded_end = QRegularExpression(QStringLiteral("\\W\\d$"));

    auto match = unpadded_start.match(user_input);
    if (match.hasMatch())
        user_input.insert(match.capturedStart(), QLatin1Char('0'));

    match = unpadded_middle.match(user_input);
    if (match.hasMatch())
        user_input.insert(match.capturedStart() + 1, QLatin1Char('0'));

    match = unpadded_end.match(user_input);
    if (match.hasMatch())
        user_input.insert(match.capturedStart() + 1, QLatin1Char('0'));
    DEB << "Padded zeroes: " << user_input;
}
// 10.10.2020
void DateTime::addSeperators(QString &user_input, const OPL::DateFormat &format)
{
    switch (format) {
    case OPL::DateFormat::ISODate:
        if (user_input.length() > 7) {
            user_input.insert(4, QLatin1Char('-'));
            user_input.insert(7, QLatin1Char('-'));
        } else {
            user_input.insert(2, QLatin1Char('-'));
            user_input.insert(5, QLatin1Char('-'));
        }
        break;
    case OPL::DateFormat::DE:
            user_input.insert(2, QLatin1Char('.'));
            user_input.insert(5, QLatin1Char('.'));
        break;
    case OPL::DateFormat::EN:
            user_input.insert(2, QLatin1Char('/'));
            user_input.insert(5, QLatin1Char('/'));
        break;
    }
    DEB << "Added Seperators: " << user_input;
}

bool DateTime::containsSeperator(const QString &user_input)
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

const QStringList& DateTime::getDisplayNames()
{
    return DISPLAY_NAMES;
}

const QString DateTime::getFormatString(OPL::DateFormat format)
{
    return DATEFORMATSMAP.value(format);
}

const QString DateTime::currentDate()
{
    return QDate::currentDate().toString(Qt::ISODate);
}

} // namespace OPL
