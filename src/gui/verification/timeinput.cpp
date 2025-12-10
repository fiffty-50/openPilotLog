#include "timeinput.h"
#include "src/classes/time.h"
#include "src/opl.h"
#include <QTime>

bool TimeInput::isValid() const
{
    return OPL::Time::fromString(input, m_format).isValid();
}

/*!
 * \brief Tries to format user input to hh:mm.
 * \details If the user input is not a valid time, this function tries to fix it. Accepted Inputs are
 * hh:mm, h:mm, hhmm or hmm. Returns "hh:mm" or an empty string if the resulting time is invalid.
 */
QString TimeInput::fixup() const
{
    switch(m_format.timeFormat()) {
    case OPL::DateTimeFormat::TimeFormat::Default:
        return fixDefaultFormat();
    case OPL::DateTimeFormat::TimeFormat::Decimal:
        return fixDecimalFormat();
    case OPL::DateTimeFormat::TimeFormat::Custom:
        return input; // custom formats cannot be fixed
        break;
    }
    return QString();
}

const QString TimeInput::fixDefaultFormat() const
{
    if(input == QString())
        return QStringLiteral("00:00");
    // try inserting a ':' for hhmm inputs
    QString fixed = input;
    if (input.contains(':')) { // contains seperator
        if(input.length() == 4)
            fixed.prepend(QLatin1Char('0'));
    } else { // does not contain seperator
        if(input.length() == 4) {
            fixed.insert(2, ':');
        }
        if(input.length() == 3) {
            fixed.prepend(QLatin1Char('0'));
            fixed.insert(2, ':');
        }
    }

    OPL::Time fixedTime = OPL::Time::fromString(fixed, m_format);
    if(fixedTime.isValid()) {
        return OPL::Time::fromString(fixed, m_format).toString();
    } else {
        return QString();
    }


}

const QString TimeInput::fixDecimalFormat() const
{
    // try to replace an erroneus decimal seperator
    QString fixed = input;
    return OPL::Time::fromString(fixed.replace(QLatin1Char(','), OPL::DECIMAL_SEPERATOR), m_format).toString();
}

