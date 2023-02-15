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
#include "src/database/database.h"
#include "dbcompletiondata.h"


namespace OPL {

QT_DEPRECATED
void DbCompletionData::init()
{
    LOG << "Initialising Completion Data...";
    // retreive user modifiable data
    pilotsIdMap = getIdMap(CompleterTarget::PilotNames);
    tailsIdMap  = getIdMap(CompleterTarget::Registrations);
    pilotList   = getCompletionList(CompleterTarget::PilotNames);
    tailsList   = getCompletionList(CompleterTarget::Registrations);

    // For tails, also provide completion for registration stripped of the '-' character
    QStringList tails_list = getCompletionList(CompleterTarget::Registrations);
    for (auto &reg : tails_list) {
        if(reg.contains(QLatin1Char('-'))) { // check to avoid duplication if reg has no '-'
            QString copy = reg;
            reg.remove(QLatin1Char('-'));
            reg = copy + " (" + reg + QLatin1Char(')');
        }
    }
    tailsList   = tails_list;

    // retreive default data
    airportIcaoIdMap = getIdMap(CompleterTarget::AirportIdentifierICAO);
    airportIataIdMap = getIdMap(CompleterTarget::AirportIdentifierIATA);
    airportNameIdMap = getIdMap(CompleterTarget::AirportNames);
    airportList      = getCompletionList(CompleterTarget::AirportIdentifier);

    // retreive default data
    airportsMapICAO = getIdMap(CompleterTarget::AirportIdentifierICAO);
    airportsMapIATA = getIdMap(CompleterTarget::AirportIdentifierIATA);
}

QT_DEPRECATED
void DbCompletionData::update()
{
        updatePilots();
        updateTails();
        updateAirports();
};

QT_DEPRECATED
void DbCompletionData::updateTails()
{
    DEB << "Updating Tails...";
    tailsIdMap  = getIdMap(CompleterTarget::Registrations);
    tailsList   = getCompletionList(CompleterTarget::Registrations);
}

QT_DEPRECATED
void DbCompletionData::updatePilots()
{
    DEB << "Updating Pilots...";
    pilotsIdMap  = getIdMap(CompleterTarget::PilotNames);
    pilotList    = getCompletionList(CompleterTarget::PilotNames);
}

QT_DEPRECATED
void DbCompletionData::updateAirports()
{
    DEB << "Updating Airports...";
    airportIcaoIdMap = getIdMap(CompleterTarget::AirportIdentifierICAO);
    airportIataIdMap = getIdMap(CompleterTarget::AirportIdentifierIATA);
    airportNameIdMap = getIdMap(CompleterTarget::AirportNames);
    airportList      = getCompletionList(CompleterTarget::AirportIdentifier);
}

QT_DEPRECATED
const QStringList DbCompletionData::getCompletionList(CompleterTarget target)
{
    QString statement;

    switch (target) {
    case CompleterTarget::PilotNames:
        statement.append(QStringLiteral("SELECT lastname||', '||firstname FROM pilots"));
        break;
    case CompleterTarget::AircraftTypes:
        statement.append(QStringLiteral("SELECT make||' '||model FROM aircraft WHERE model IS NOT NULL AND variant IS NULL "
                         "UNION "
                         "SELECT make||' '||model||'-'||variant FROM aircraft WHERE variant IS NOT NULL"));
        break;
    case CompleterTarget::AirportIdentifier:
        statement.append(QStringLiteral("SELECT icao FROM airports UNION SELECT iata FROM airports"));
        break;
    case CompleterTarget::Registrations:
        statement.append(QStringLiteral("SELECT registration FROM tails"));
        break;
    case CompleterTarget::Companies:
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

QT_DEPRECATED
const QHash<int, QString> DbCompletionData::getIdMap(CompleterTarget target)
{
    QString statement;

    switch (target) {
    case CompleterTarget::PilotNames:
        statement.append(QStringLiteral("SELECT ROWID, lastname||', '||firstname FROM pilots"));
        break;
    case CompleterTarget::AircraftTypes:
        statement.append(QStringLiteral("SELECT ROWID, make||' '||model FROM aircraft WHERE model IS NOT NULL AND variant IS NULL "
                         "UNION "
                         "SELECT ROWID, make||' '||model||'-'||variant FROM aircraft WHERE variant IS NOT NULL"));
        break;
    case CompleterTarget::AirportIdentifierICAO:
        statement.append(QStringLiteral("SELECT ROWID, icao FROM airports"));
        break;
    case CompleterTarget::AirportIdentifierIATA:
        statement.append(QStringLiteral("SELECT ROWID, iata FROM airports WHERE iata NOT NULL"));
        break;
    case CompleterTarget::AirportNames:
        statement.append(QStringLiteral("SELECT ROWID, name FROM airports"));
        break;
    case CompleterTarget::Registrations:
        statement.append(QStringLiteral("SELECT ROWID, registration FROM tails"));
        break;
    default:
        return {};
    }

    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare(statement);
    query.exec();

    auto id_map = QHash<int, QString>();
    while (query.next())
        id_map.insert(query.value(0).toInt(), query.value(1).toString());
    return id_map;
}

QT_DEPRECATED
const QHash<int, QString> &DbCompletionData::getAirportsMapICAO() const
{
    return airportsMapICAO;
}

QT_DEPRECATED
const QHash<int, QString> &DbCompletionData::getAirportsMapIATA() const
{
    return airportsMapIATA;
}
} // namespace OPL
