/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#ifndef DATABASECACHE_H
#define DATABASECACHE_H
#include "src/opl.h"
#include <QtCore>

namespace OPL{

using IdMap = QHash<int, QString>;
using KeyMap = QHash<QString, int>;
using KeyMultiMap = QMultiMap<QString, int>;
#define DBCache OPL::DatabaseCache::instance()

/*!
 * \brief Caches certain often accessed database content in memory
 * \details Access to the database is rather slow and memory these days is cheap enough to cache some contents for ease
 * and speed of access. This class provides lists of database entries that are used as inputs for QCompleter instances
 * as well as maps which contain database entries and their associated row ids, which are used for user input verification.
 *
 * The Cache can be accessed by using the DBCACHE macro and needs to be updated whenever the database contents are modified.
 */
class DatabaseCache : public QObject
{
public:
    static DatabaseCache* instance() {
        static DatabaseCache instance;
        return &instance;
    }

    DatabaseCache(DatabaseCache const&) = delete;
    void operator=(DatabaseCache const&) = delete;

    enum CompleterTarget {PilotNames, Tails, AircraftTypeStrings, AirportCodesAnyType, AirportsICAO, AirportNames, AirportsIATA, Companies, Types};

    void init();

    const IdMap &getAirportsMapICAO() const;
    const IdMap &getAirportsMapIATA() const;
    const IdMap &getPilotNamesMap() const;
    const IdMap &getTailsMap() const;
    const IdMap &getTypesMap() const;
    const IdMap &getAircraftMap() const;
    const IdMap &getAirportsMapNames() const;

    enum class MapType { AirportCodesIcao,
                         AirportCodesIata,
                         AirportNames,
                         AircraftTypes,
                         TailRegistrations,
                         PilotNames,
    };

    enum class ListType { PilotNames,
                          AircraftTypes,
                          Tails,
                          AirportCodes,
                          Companies,
    };

    const QStringList &getList(ListType type);
    const IdMap &getMap(MapType type);
    const KeyMap &getKeyMap(MapType type);


private:
    Q_OBJECT
    DatabaseCache() {};

    // Id Maps

    /*!
     * \brief key: airport_id / value: icao_code
     */
    IdMap airportsIcaoMap;
    /*!
     * \brief key: airport_id / value: iata_code
     */
    IdMap airportsIataMap;
    /*!
     * \brief key: airport_id / value: airport_name
     */
    IdMap airportNamesMap;
    /*!
     * \brief key: pilot_id / value: pilot_name
     */
    IdMap pilotNamesMap;
    /*!
     * \brief key: tail_id / value: registration
     */
    IdMap tailsRegistrationMap;
    /*!
     * \brief key: aircraft_type_id / value :
     */
    IdMap aircraftTypesMap;

    void convertIdMapToKeyMap(IdMap &idMap, KeyMap& keyMap);


    // Key Maps - Those are equivalent to the ID maps but key and value are reversed
    KeyMap airportsIcaoKeyMap;
    KeyMap airportsIataKeyMap;
    KeyMap airportNamesKeyMap;
    KeyMap pilotNamesKeyMap;
    KeyMap tailsRegistrationKeyMap;
    KeyMap aircraftTypesKeyMap;

    // Key Multi Maps - used for reverse IdMaps where one key can have several values
    // -- all airport codes to airport_id


    // Lists
    QStringList pilotNamesList;
    QStringList aircraftTailsList;
    QStringList aircraftTypesList;
    QStringList airportCodesList;
    QStringList companiesList;

    // Query the database to update a map
    const IdMap fetchMap(MapType target);
    const QStringList fetchList(ListType target);

    void updateTails();
    void updateAirports();
    void updatePilots();
    void updateAircraftTypes();

public slots:
    void onDatabaseUpdated(const OPL::DbTable table);
signals:
    void databaseCacheUpdated(const OPL::DbTable table);

};

}// namespace OPL
#endif // DATABASECACHE_H
