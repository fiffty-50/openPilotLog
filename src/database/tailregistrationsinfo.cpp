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
#include "tailregistrationsinfo.h"
#include "src/database/database.h"
#include <QSqlQuery>

TailRegistrationsInfo::TailRegistrationsInfo(QObject *parent) : QObject{parent}
{
    refresh();

    connect(DB, &OPL::Database::dataBaseUpdated, this, [this](OPL::DbTable table) {
        if (table == OPL::DbTable::v2AircraftTails) refresh();
    });
}

void TailRegistrationsInfo::refresh()
{
    m_registrationToId.clear();
    m_idToRegistration.clear();

    QSqlQuery query;
    query.exec("SELECT ROWID, registration FROM aircraft_tails");

    while (query.next()) {
        int id      = query.value(0).toInt();
        QString reg = query.value(1).toString();

        if (!reg.isEmpty()) {
            m_registrationToId.insert(reg, id);
            m_idToRegistration.insert(id, reg);
        }
    }
}

int TailRegistrationsInfo::tailId(const QString &registration)
{
    return m_registrationToId.value(registration, 0);
}

QString TailRegistrationsInfo::registration(int tailId)
{
    return m_idToRegistration.value(tailId, {});
}
