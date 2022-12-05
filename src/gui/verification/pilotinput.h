#ifndef PILOTINPUT_H
#define PILOTINPUT_H

#include "userinput.h"

class PilotInput : public UserInput
{
public:
    PilotInput() = delete;
    PilotInput(const QString& userInput) : UserInput(userInput) {}

    /*!
     * \brief Checks if a user-given Pilot Name is present in the database
     * \details An input String is compared against a Hash Map of values present
     * in the database, if an exact match is found, the input is considered valid.
     */
    bool isValid() const override;
    /*!
     * \brief Tries to match a user given input to values present in the database.
     * \details The user input is compared to a list of values present in the database
     * and the closest match returned, or an empty string if none found.
     */
    QString fixup() const override;
private:
    const static inline QLatin1String self{"self"};
};

#endif // PILOTINPUT_H
