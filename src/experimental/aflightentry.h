#ifndef AFLIGHTENTRY_H
#define AFLIGHTENTRY_H

#include "src/experimental/aentry.h"
#include "src/functions/acalc.h"

namespace experimental {

class AFlightEntry : public AEntry {
public:
    AFlightEntry();
    AFlightEntry(const AFlightEntry& pe) = default;
    AFlightEntry& operator=(const AFlightEntry& pe) = default;
    AFlightEntry(int row_id);
    AFlightEntry(TableData table_data);

    enum pilot {pic, sic, thirdPilot };

    /*!
     * \brief Returs a summary of the flight data
     * \return "doft, dept, tofb, dest, tonb"
     */
    const QString summary();
    /*!
     * \brief Returns the tails' registration from the database.
     */
    const QString registration();
    /*!
     * \brief Returns the pilots name from the Database
     *
     * \param pilot_number - 1=pic, 2=second Pilot, 3 = third Pilot
     * \return "Lastname, Firstname"
     */
    const QString pilotName(pilot);
};

} // namespace experimental

#endif // AFLIGHTENTRY_H
