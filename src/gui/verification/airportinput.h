#ifndef AIRPORTINPUT_H
#define AIRPORTINPUT_H

#include "userinput.h"

/*!
 * \brief Checks if a user input is a valid airport code
 * \details The user can input either ICAO or IATA Airport Codes, but both will be
 * matched against ICAO codes, which are more widespread and used exclusively in the database
 * because there are airports which don't have an IATA code, mainly smaller non-commercial ones.
 *
 * The AirportInput class compares an input string against a Hash Map of Airport Codes and their respective
 * row id's, which are cached from the database.
 */
class AirportInput : public UserInput
{
public:
    AirportInput() = delete;
    AirportInput(const QString &input) : UserInput(input) {}

    /*!
     * \brief An input is considered valid if it can be matched to an ICAO code present in the database
     */
    bool isValid() const override;

    /*!
     * \brief fixup tries to convert IATA to ICAO code and converts lower to upper case
     * \details The input is converted to upper case and then compared against ICAO and IATA
     * codes present in the database. If a match is found, the valid ICAO code from the database
     * cache is returned, otherwise an empty QString.
     */
    QString fixup() const override;

};

#endif // AIRPORTINPUT_H
