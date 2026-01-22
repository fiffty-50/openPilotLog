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
#include "databasecache.h"
#include "src/database/database.h"
#include "src/opl.h"
#include <QSqlQuery>

namespace OPL {

void DatabaseCache::init()
{
    LOG << "Initialising database cache...";

    updateTails();
    updatePilots();
    updateAirports();
    updateAircraftTypes();

    // Listen to database for updates, reload cache if needed
    QObject::connect(DB, &OPL::Database::dataBaseUpdated, this,
                     &OPL::DatabaseCache::onDatabaseUpdated);
}

const IdMap DatabaseCache::fetchMap(MapType target)
{
    QString statement;

    switch (target) {
    case MapType::AirportCodesIcao:
        statement =
            (QStringLiteral("SELECT "
                            "airport_id, "
                            "airport_code "
                            "FROM airport_codes "
                            "WHERE (valid_to_jd IS NULL OR valid_to_jd >= julianday('now')) "
                            "AND valid_from_jd <= julianday('now') "
                            "AND code_type = 'ICAO';"));
        break;
    case MapType::AirportCodesIata:
        statement =
            (QStringLiteral("SELECT "
                            "airport_id, "
                            "airport_code "
                            "FROM airport_codes "
                            "WHERE (valid_to_jd IS NULL OR valid_to_jd >= julianday('now')) "
                            "AND valid_from_jd <= julianday('now') "
                            "AND code_type = 'IATA';"));
        break;
    case MapType::AirportNames:
        statement.append(QStringLiteral("SELECT ROWID, airport_name FROM airports"));
        break;
    case MapType::PilotNames:
        statement.append(QStringLiteral("SELECT ROWID, pilot_name FROM pilots"));
        break;
    case MapType::TailRegistrations:
        statement.append(QStringLiteral("SELECT ROWID, registration FROM aircraft_tails"));
        break;
    case MapType::AircraftTypes:
        statement.append(QStringLiteral("SELECT ROWID, make||' '||model AS ident "
                                        "FROM aircraft_types "
                                        "WHERE model IS NOT NULL AND variant IS NULL "
                                        "UNION "
                                        "SELECT ROWID, make||' '||model||'-'||variant "
                                        "FROM aircraft_types "
                                        "WHERE variant IS NOT NULL"));
        break;
    default:
        return {};
    }

    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare(statement);
    query.exec();

    IdMap id_map;
    while (query.next())
        id_map.insert(query.value(0).toInt(), query.value(1).toString());
    return id_map;
}

const QStringList DatabaseCache::fetchList(ListType target)
{
    QString statement;

    switch (target) {
    case ListType::PilotNames:
        statement.append(QStringLiteral("SELECT pilot_name FROM pilots"));
        break;
    case ListType::AircraftTypes:
        statement.append(QStringLiteral("SELECT make||' '||model AS ident "
                                        "FROM aircraft_types "
                                        "WHERE model IS NOT NULL AND variant IS NULL "
                                        "UNION "
                                        "SELECT make||' '||model||'-'||variant AS ident "
                                        "FROM aircraft_types "
                                        "WHERE variant IS NOT NULL"));
        break;
    case ListType::AirportCodes:
        statement.append(
            QStringLiteral("WITH CurrentCode AS ( "
                           "SELECT "
                           "airport_code, "
                           "valid_from_jd, "
                           "valid_to_jd "
                           "FROM airport_codes "
                           "WHERE (valid_to_jd IS NULL OR valid_to_jd >= julianday('now')) "
                           "AND valid_from_jd <= julianday('now')) "
                           "SELECT "
                           "airport_code "
                           "FROM CurrentCode "));
        break;
    case ListType::Tails:
        statement.append(QStringLiteral("SELECT registration FROM aircraft_tails"));
        break;
    case ListType::Companies:
        statement.append(QStringLiteral("SELECT company FROM pilots"));
        break;
    default:
        Q_UNREACHABLE();
        return QStringList();
    }

    QSqlQuery query;
    query.prepare(statement);
    query.setForwardOnly(true);
    query.exec();

    QStringList completer_list;
    while (query.next())
        completer_list.append(query.value(0).toString());

    completer_list.sort();
    completer_list.removeAll(QString());
    completer_list.removeDuplicates();

    return completer_list;
}

const KeyMultiMap DatabaseCache::fetchMultiMap(MapType target)
{
    QString statement;
    switch (target) {
    case MapType::AirportCodesAll:
        statement = QStringLiteral("WITH CurrentCode AS ( "
                                   "SELECT "
                                   "airport_code, "
                                   "airport_id, "
                                   "valid_from_jd, "
                                   "valid_to_jd "
                                   "FROM airport_codes "
                                   "WHERE (valid_to_jd IS NULL OR valid_to_jd >= julianday('now')) "
                                   "AND valid_from_jd <= julianday('now')) "
                                   "SELECT "
                                   "airport_code, "
                                   "airport_id "
                                   "FROM CurrentCode ");
        break;
    default:
        break;
    }

    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare(statement);
    query.exec();

    KeyMultiMap map;
    while (query.next())
        map.insert(query.value(0).toString(), query.value(1).toInt());
    return map;
}

void DatabaseCache::updateTails()
{
    tailsRegistrationMap = fetchMap(MapType::TailRegistrations);
    convertIdMapToKeyMap(tailsRegistrationMap, tailsRegistrationKeyMap);

    aircraftTailsList = fetchList(ListType::Tails);
}

void DatabaseCache::updateAirports()
{
    airportsIataMap = fetchMap(MapType::AirportCodesIata);
    convertIdMapToKeyMap(airportsIataMap, airportsIataKeyMap);

    airportsIcaoMap = fetchMap(MapType::AirportCodesIcao);
    convertIdMapToKeyMap(airportsIcaoMap, airportsIcaoKeyMap);

    airportNamesMap = fetchMap(MapType::AirportNames);
    convertIdMapToKeyMap(airportNamesMap, airportNamesKeyMap);

    airportCodesAllMultiMap = fetchMultiMap(MapType::AirportCodesAll);

    airportCodesList = fetchList(ListType::AirportCodes);
}

void DatabaseCache::updatePilots()
{
    pilotNamesMap  = fetchMap(MapType::PilotNames);
    pilotNamesList = fetchList(ListType::PilotNames);
    companiesList  = fetchList(ListType::Companies);
}

void DatabaseCache::updateAircraftTypes()
{
    aircraftTypesList = fetchList(ListType::AircraftTypes);
    aircraftTypesMap  = fetchMap(MapType::AircraftTypes);

    convertIdMapToKeyMap(aircraftTypesMap, aircraftTypesKeyMap);
}

void DatabaseCache::onDatabaseUpdated(const OPL::DbTable table)
{
    LOG << "Updating Database Cache...";
    switch (table) {
    case DbTable::v2Pilots:
        updatePilots();
        break;
    case DbTable::v2AircraftTails:
        updateTails();
        break;
    case DbTable::v2AircraftTypes:
        updateAircraftTypes();
        break;
    case DbTable::v2Airports:
        updateAirports();
        break;
    case DbTable::v2AirportCodes:
        updateAirports();
        break;
    default:
        break;
    }
    emit databaseCacheUpdated(table);
}

const IdMap &DatabaseCache::getAirportsMapICAO() const { return airportsIcaoMap; }

const IdMap &DatabaseCache::getAirportsMapIATA() const { return airportsIataMap; }

const IdMap &DatabaseCache::getPilotNamesMap() const { return pilotNamesMap; }

const QStringList &DatabaseCache::getList(ListType type)
{
    switch (type) {
    case ListType::PilotNames:
        return pilotNamesList;
        break;
    case ListType::AircraftTypes:
        return aircraftTypesList;
        break;
    case ListType::Tails:
        return aircraftTailsList;
        break;
    case ListType::AirportCodes:
        return airportCodesList;
        break;
    case ListType::Companies:
        return companiesList;
        break;
    }
    Q_UNREACHABLE();
}

const IdMap &DatabaseCache::getMap(MapType type)
{
    switch (type) {
    case MapType::AirportCodesIcao:
        return airportsIcaoMap;
        break;
    case MapType::AirportCodesIata:
        return airportsIataMap;
        break;
    case MapType::AirportNames:
        return airportNamesMap;
        break;
    case MapType::AircraftTypes:
        return aircraftTypesMap;
        break;
    case MapType::TailRegistrations:
        return tailsRegistrationMap;
        break;
    case MapType::PilotNames:
        return pilotNamesMap;
        break;
    }
    Q_UNREACHABLE();
}

const KeyMap &DatabaseCache::getKeyMap(MapType type)
{
    switch (type) {
    case MapType::AirportCodesIcao:
        return airportsIcaoKeyMap;
        break;
    case MapType::AirportCodesIata:
        return airportsIataKeyMap;
        break;
    case MapType::AirportNames:
        return aircraftTypesKeyMap;
        break;
    case MapType::AircraftTypes:
        return aircraftTypesKeyMap;
        break;
    case MapType::TailRegistrations:
        return tailsRegistrationKeyMap;
        break;
    case MapType::PilotNames:
        return pilotNamesKeyMap;
        break;
    }
    Q_UNREACHABLE();
}

const KeyMultiMap &DatabaseCache::getMultiMap(MapType type)
{
    switch (type) {
    case MapType::AirportCodesAll:
        return airportCodesAllMultiMap;
        break;
    default:
        return {};
    }
}

void DatabaseCache::convertIdMapToKeyMap(IdMap &idMap, KeyMap &keyMap)
{
    keyMap.clear();
    keyMap.reserve(idMap.size());
    for (auto it = idMap.constBegin(); it != idMap.constEnd(); ++it) {
        keyMap.insert(it.value(), it.key());
    }
}

} // namespace OPL
