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

namespace OPL{

void DatabaseCache::init()
{
    LOG << "Initialising database cache...";

    updateTails();
    updatePilots();
    updateAirports();
    updateSimulators();
    updateAircraft();

    // Listen to database for updates, reload cache if needed
    QObject::connect(DB,   		   &OPL::Database::dataBaseUpdated,
                     this,         &OPL::DatabaseCache::onDatabaseUpdated);

}

const IdMap DatabaseCache::fetchMap(CompleterTarget target)
{
    QString statement;

    switch (target) {
    case AirportsICAO:
        statement.append(QStringLiteral("SELECT ROWID, icao FROM airports"));
        break;
    case AirportsIATA:
        statement.append(QStringLiteral("SELECT ROWID, iata FROM airports WHERE iata NOT NULL"));
        break;
    case AirportNames:
        statement.append(QStringLiteral("SELECT ROWID, name FROM airports"));
        break;
    case PilotNames:
        statement.append(QStringLiteral("SELECT ROWID, lastname||', '||firstname FROM pilots"));
        break;
    case Tails:
        statement.append(QStringLiteral("SELECT ROWID, registration FROM tails"));
        break;
    case Types:
        statement.append(QStringLiteral("SELECT ROWID, make||' '||model FROM tails WHERE model IS NOT NULL AND variant IS NULL "
                                        " UNION "
                                        " SELECT ROWID, make||' '||model||'-'||variant FROM tails WHERE variant IS NOT NULL"));
        break;
    case AircraftTypes:
        statement.append(QStringLiteral("SELECT ROWID, make||' '||model FROM aircraft WHERE model IS NOT NULL AND variant IS NULL "
                         "UNION "
                         "SELECT ROWID, make||' '||model||'-'||variant FROM aircraft WHERE variant IS NOT NULL"));
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

const QStringList DatabaseCache::fetchList(CompleterTarget target)
{
    QString statement;

    switch (target) {
    case PilotNames:
        statement.append(QStringLiteral("SELECT lastname||', '||firstname FROM pilots"));
        break;
    case AircraftTypes:
        statement.append(QStringLiteral("SELECT make||' '||model FROM aircraft WHERE model IS NOT NULL AND variant IS NULL "
                         "UNION "
                         "SELECT make||' '||model||'-'||variant FROM aircraft WHERE variant IS NOT NULL"));
        break;
    case AirportsAny:
        statement.append(QStringLiteral("SELECT icao FROM airports UNION SELECT iata FROM airports"));
        break;
    case Tails:
        statement.append(QStringLiteral("SELECT registration FROM tails"));
        break;
    case Companies:
        statement.append(QStringLiteral("SELECT company FROM pilots"));
        break;
    default:
        DEB << "Not a valid completer target for this function.";
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

void DatabaseCache::updateTails()
{
    tailsMap = fetchMap(Tails);
    tailsList = fetchList(Tails);
    for (auto &reg : tailsList) {
        if(reg.contains(QLatin1Char('-'))) { // check to avoid duplication if reg has no '-'
            QString copy = reg;
            reg.remove(QLatin1Char('-'));
            reg = copy + " (" + reg + QLatin1Char(')');
        }
    }
    typesMap = fetchMap(Types);
}

void DatabaseCache::updateAirports()
{
    airportsMapIATA  = fetchMap(AirportsIATA);
    airportsMapICAO  = fetchMap(AirportsICAO);
    airportsMapNames = fetchMap(AirportNames);
    airportList      = fetchList(AirportsAny);
}

void DatabaseCache::updateSimulators()
{
    TODO << "Simulators map not yet cached";
}

void DatabaseCache::updatePilots()
{
    pilotNamesMap  = fetchMap(PilotNames);
    pilotNamesList = fetchList(PilotNames);
    companiesList  = fetchList(Companies);
}

void DatabaseCache::updateAircraft()
{
    aircraftList = fetchList(AircraftTypes);
    aircraftMap = fetchMap(AircraftTypes);
}

void DatabaseCache::onDatabaseUpdated(const OPL::DbTable table)
{
    LOG << "Updating Database Cache...";
    switch (table) {
    case DbTable::Pilots:
        updatePilots();
        break;
    case DbTable::Tails:
        updateTails();
        break;
    case DbTable::Simulators:
        updateSimulators();
        break;
    case DbTable::Airports:
        updateAirports();
        break;
    case DbTable::Aircraft:
        updateAircraft();
        break;
    default:
        break;
    }
    emit databaseCacheUpdated(table);
}
const IdMap &DatabaseCache::getAirportsMapICAO() const
{
    return airportsMapICAO;
}

const IdMap &DatabaseCache::getAirportsMapIATA() const
{
    return airportsMapIATA;
}

const IdMap &DatabaseCache::getPilotNamesMap() const
{
    return pilotNamesMap;
}

const QStringList &DatabaseCache::getPilotNamesList() const
{
    return pilotNamesList;
}

const QStringList &DatabaseCache::getTailsList() const
{
    return tailsList;
}

const QStringList &DatabaseCache::getAirportList() const
{
    return airportList;
}

const QStringList &DatabaseCache::getCompaniesList() const
{
    return companiesList;
}

const QStringList &DatabaseCache::getAircraftList() const
{
    return aircraftList;
}

const IdMap &DatabaseCache::getAircraftMap() const
{
    return aircraftMap;
}

const IdMap &DatabaseCache::getAirportsMapNames() const
{
    return airportsMapNames;
}

const IdMap &DatabaseCache::getTailsMap() const
{
    return tailsMap;
}

const IdMap &DatabaseCache::getTypesMap() const
{
    return typesMap;
}



} // namespace OPL
