#ifndef ACOMPLETIONDATA_H
#define ACOMPLETIONDATA_H
#include "src/database/adatabase.h"

/*!
 * \brief The ACompletionData class provides data to QCompleters and QMaps used
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
    QMap<PilotName_T, PilotRowId_T> pilotsIdMap;
    QMap<TailRegistration_T, TailId_T> tailsIdMap;
    QMap<AirportICAO_T, AirportId_T> airportIcaoIdMap;
    QMap<AirportIATA_T, AirportId_T> airportIataIdMap;
    QMap<AirportName_T, AirportId_T> airportNameIdMap;
    // Lists for QCompleter
    QStringList pilotList;
    QStringList tailsList;
    QStringList airportList;
};

#endif // ACOMPLETIONDATA_H
