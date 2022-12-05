/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef DBCOMPLETIONDATA_H
#define DBCOMPLETIONDATA_H
#include "src/opl.h"

namespace OPL {

/*!
 * \brief The DatabaseTarget enum lists database items that are
 * used by completers, for content matching or need to be accessed programatically.
 */
enum class CompleterTarget
{
    AirportIdentifierICAO,
    AirportIdentifierIATA,
    AirportIdentifier,
    AirportNames,
    Registrations,
    Companies,
    PilotNames,
    AircraftTypes
};

/*!
 * \brief Provides data for QCompleters and QHashs
 *
 * \details QCompleters and QHashes are used for mapping user input to database keys.
 * The required data is retreived from the database and cached for fast access without the
 * need to query the database. The DbCompletionData class holds the last state of the database
 * (state of user-modifiable data) in order to intelligently refresh the completion data when needed.
 */
class DbCompletionData
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
    void updateAirports();

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

    /*!
     * \brief getCompletionList returns a QStringList of values for a
     * QCompleter based on database values
     */
    static const QStringList getCompletionList(CompleterTarget target);

    /*!
     * \brief returns a QHash of a human-readable database value and
     * its row id. Used in the Dialogs to map user input to unique database entries.
     */
    static const QHash<int, QString> getIdMap(CompleterTarget target);

    const QHash<int, QString> &getAirportsMapICAO() const;
    const QHash<int, QString> &getAirportsMapIATA() const;

private:
    QHash<int, QString> airportsMapICAO;
    QHash<int, QString> airportsMapIATA;

};
} // namespace OPL

#endif // DBCOMPLETIONDATA_H
