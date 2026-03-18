/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
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
#include "aircrafttypesinfo.h"
#include "src/database/database.h"
#include <QSqlQuery>

AircraftTypesInfo::AircraftTypesInfo(QObject *parent) : QObject{parent}
{
    refresh();

    connect(DB, &OPL::Database::databaseUpdated, this, [this](OPL::DbTable table) {
        if (table == OPL::DbTable::AircraftTypes) refresh();
    });
}

void AircraftTypesInfo::refresh()
{
    int size = m_idToIdent.size() == 0 ? 64 : m_idToIdent.size();
    m_identToId.clear();
    m_identToId.reserve(size);
    m_idToIdent.clear();
    m_idToIdent.reserve(size);

    QSqlQuery query;
    query.exec(QStringLiteral("SELECT ROWID, make||' '||model AS ident "
                              "FROM aircraft_types "
                              "WHERE model IS NOT NULL AND variant IS NULL "
                              "UNION "
                              "SELECT ROWID, make||' '||model||'-'||variant "
                              "FROM aircraft_types "
                              "WHERE variant IS NOT NULL"));

    while (query.next()) {
        int id        = query.value(0).toInt(); // ROWID
        QString ident = query.value(1).toString();

        if (!ident.isEmpty()) {
            m_identToId.insert(ident, id);
            m_idToIdent.insert(id, ident);
            m_typeStringmap.insert(ident, id);
        }
    }
}

int AircraftTypesInfo::id(const QString &type_string) { return m_identToId.value(type_string, 0); }

QString AircraftTypesInfo::typeString(int type_id) { return m_idToIdent.value(type_id, {}); }

bool AircraftTypesInfo::contains(int type_id) const { return m_idToIdent.contains(type_id); }
