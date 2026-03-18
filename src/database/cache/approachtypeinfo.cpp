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
#include "approachtypeinfo.h"
#include "src/database/database.h"
#include "src/opl.h"

ApproachTypeInfo::ApproachTypeInfo(QObject *parent) : QObject{parent}
{

    refreshIndices();

    connect(DB, &OPL::Database::databaseUpdated, this, [this](OPL::DbTable table) {
        if (table == OPL::DbTable::ApproachTypes) {
            LOG << "Updating ApproachTypes Info.";
            refreshIndices();
        }
    });
}
void ApproachTypeInfo::refreshIndices()
{
    m_approachTypeIdMap.clear();
    m_approachNameMap.clear();

    auto query = SELECT_ALL.arg(OPL::GLOBALS->getDbTableName(OPL::DbTable::ApproachTypes));
    QSqlQuery q;
    q.prepare(query);
    if (!q.exec()) DEB << "Query failed: " << q.lastQuery();

    while (q.next()) {
        int id       = q.value(COLUMN_TYPE_ID).toInt();
        QString name = q.value(COLUMN_APPROACH_NAME).toString();
        m_approachNameMap.insert(name, id);
        m_approachTypeIdMap.insert(id, name);
    }
}

int ApproachTypeInfo::idFromName(const QString &approach_name) const
{
    return m_approachNameMap.value(approach_name);
}

QString ApproachTypeInfo::nameFromId(int approach_id) const
{
    return m_approachTypeIdMap.value(approach_id);
}
