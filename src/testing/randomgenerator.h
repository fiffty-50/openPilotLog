#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include "src/database/row.h"
namespace OPL {

/*!
 * \brief The RandomGenerator class generates random data for testing and debugging purposes.
 * \details Using the randomFlight() method it is possible to generate a random flight with
 * pilot and tail details already present in the database. For this to work, there cannot be any
 * gaps in the database, i.e. the row_id must be rising uninterruptedly. If this cannot be guaranteed,
 * use pass safe_mode = true as an argument when creating instantiating the generator.
 */
class RandomGenerator
{
public:
    RandomGenerator(bool safe_mode = false);

    const FlightEntry randomFlight();

    const QTime randomTime();
    const QDate randomDate();
    const QDateTime randomDateTime();
    const QString randomAirport();
    const int randomPilot();
    const int randomTail();
    const bool randomBool();


private:
    bool safeMode = false;
    const static inline QStringList m_function_times = {
        OPL::Db::FLIGHTS_TPIC,
        OPL::Db::FLIGHTS_TPICUS,
        OPL::Db::FLIGHTS_TSIC,
        OPL::Db::FLIGHTS_TDUAL,
        OPL::Db::FLIGHTS_TFI,
    };

    int m_numberOfPilots;
    int m_numberOfAirports;
    int m_numberOfTails;
};

} // namespace OPL
#endif // RANDOMGENERATOR_H
