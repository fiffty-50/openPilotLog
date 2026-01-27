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
#include "pilotinfo.h"
#include "src/database/database.h"
#include "src/opl.h"

PilotsInfo::PilotsInfo(QObject *parent) : QObject{parent}, m_model(new QSqlTableModel(this))
{
    m_model->setTable(OPL::GLOBALS->getDbTableName(OPL::DbTable::v2Pilots));
    m_model->select();

    while (m_model->canFetchMore())
        m_model->fetchMore();

    refreshIndices();
    connect(DB, &OPL::Database::dataBaseUpdated, this, [this](OPL::DbTable table) {
        if (table == OPL::DbTable::v2Pilots) {
            m_model->select();
            while (m_model->canFetchMore())
                m_model->fetchMore();
            refreshIndices();
        }
    });
}

void PilotsInfo::refreshIndices()
{
    int rows = m_model->rowCount();

    m_pilotIdToRow.clear();
    m_nameToPilotId.clear();

    m_pilotIdToRow.reserve(rows);
    m_nameToPilotId.reserve(rows);

    for (int row = 0; row < rows; ++row) {
        int pilotId  = m_model->data(m_model->index(row, COLUMN_PILOT_ID)).toInt();
        QString name = m_model->data(m_model->index(row, COLUMN_NAME)).toString();

        m_pilotIdToRow.insert(pilotId, row);
        if (!name.isEmpty()) m_nameToPilotId.insert(name, pilotId);
    }
}

QString PilotsInfo::dataForPilotId(int pilotId, int column) const
{
    int row = m_pilotIdToRow.value(pilotId, -1);
    return m_model->data(m_model->index(row, column)).toString();
}

// Getters

QString PilotsInfo::name(int pilotId) { return dataForPilotId(pilotId, COLUMN_NAME); }

QString PilotsInfo::alias(int pilotId) { return dataForPilotId(pilotId, COLUMN_ALIAS); }

QString PilotsInfo::company(int pilotId) { return dataForPilotId(pilotId, COLUMN_COMPANY); }

QString PilotsInfo::phone(int pilotId) { return dataForPilotId(pilotId, COLUMN_PHONE); }

QString PilotsInfo::email(int pilotId) { return dataForPilotId(pilotId, COLUMN_EMAIL); }

QString PilotsInfo::remarks(int pilotId) { return dataForPilotId(pilotId, COLUMN_REMARKS); }

int PilotsInfo::idFromName(const QString &name) { return m_nameToPilotId.value(name, 0); }
