#ifndef ACOMPLETIONDATA_H
#define ACOMPLETIONDATA_H
#include "src/database/database.h"

/*!
 * \brief The ACompletionData class provides data to QCompleters and QHashs used
 * for mapping user input to database keys. It retreives the completer targets
 * and mapping data from the database and caches it for fast access without the
 * need to query the database.
 */
class ACompletionData
{
public:  
    /*!
     * \brief init Retrieves Data and populates Lists and Maps
     */
    void init();

    /*!
     * \brief updates data from the user modifiable tables
     */
    void update();
    void updateTails();
    void updatePilots();


    // Maps for input mapping DB key - user input
    QHash<int, QString> pilotsIdMap;
    QHash<int, QString> tailsIdMap;
    QHash<int, QString> airportIcaoIdMap;
    QHash<int, QString> airportIataIdMap;
    QHash<int, QString> airportNameIdMap;
    // Lists for QCompleter
    QStringList pilotList;
    QStringList tailsList;
    QStringList airportList;
    // User Data State to trigger update if needed
    UserDataState current_state;
};

#endif // ACOMPLETIONDATA_H
