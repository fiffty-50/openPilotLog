#ifndef TIMEINPUT_H
#define TIMEINPUT_H

#include "src/opl.h"
#include "userinput.h"

class TimeInput : public UserInput
{
public:
    TimeInput() = delete;
    TimeInput(const QString &input, const OPL::DateTimeFormat &format)
        : UserInput(input), m_format(format) {}

    /*!
     * \brief Checks if a user entered String is a valid time input
     * \details A user input is considered a valid time, if it complies with the ISO-8601 standard,
     * and a valid QTime object can be derived from it.
     */
    bool isValid() const override;

    /*!
     * \brief Tries to convert a user input String into a valid time string
     * \details The user can input time in various formats, for example "hmm",
     * "hhmm" or "hh:mm". The fixup function tries to interpret the input string
     * and converts it into a QTime Object. If the resulting Time Object is valid,
     * the ISO-8601 representation "hh:mm" is returned, otherwise an empty QString.
     */
    QString fixup() const override;
private:
    const OPL::DateTimeFormat &m_format;

    const QString fixDefaultFormat() const;
    const QString fixDecimalFormat() const;
};

#endif // TIMEINPUT_H
