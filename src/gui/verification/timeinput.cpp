#include "timeinput.h"
#include "src/opl.h"
#include <QTime>

bool TimeInput::isValid() const
{
     return QTime::fromString(input, OPL::Format::TIME_FORMAT).isValid();
}

/*!
 * \brief Tries to format user input to hh:mm.
 * \details If the user input is not a valid time, this function tries to fix it. Accepted Inputs are
 * hh:mm, h:mm, hhmm or hmm. Returns "hh:mm" or an empty string if the resulting time is invalid.
 */
QString TimeInput::fixup() const
{
    QString fixed = input;

    if (input.contains(':')) { // contains seperator
        if(input.length() == 4)
            fixed.prepend('0');
    } else { // does not contain seperator
        if(input.length() == 4) {
            fixed.insert(2, ':');
        }
        if(input.length() == 3) {
            fixed.prepend('0');
            fixed.insert(2, ':');
        }
    }

    QTime time = QTime::fromString(fixed, OPL::Format::TIME_FORMAT);
    return time.toString(OPL::Format::TIME_FORMAT);
}
