#ifndef USERINPUT_H
#define USERINPUT_H
#include <QString>

/*!
 * \brief The UserInput class verifies and tries to correct user input.
 * \details Basic User Input Verification is accomplished via QCompleters, QValidators and input masks. However, in many
 * cases more complex logic is required to make sure user input is not just syntactically acceptable but also correct before
 * submitting it to the database.
 */
class UserInput
{
public:
    UserInput(const QString& input)
        : input(input) {};
    /*!
     * \brief isValid determines if the user input is valid.
     * \return
     */
    virtual bool isValid() const = 0;

    /*!
     * \brief try to correct the user input and return a corrected QString. If the user input cannot be corrected, return an empty string.
     */
    virtual QString fixup() const = 0;
protected:
    const QString& input;
};

#endif // USERINPUT_H
