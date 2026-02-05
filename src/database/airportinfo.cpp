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
#include "airportinfo.h"
#include "src/database/database.h"
#include "src/opl.h"
#include <qsqltablemodel.h>

AirportInfo::AirportInfo(QObject *parent) : QObject{parent}, m_model(new QSqlTableModel(this))
{
    m_model->setTable(OPL::GLOBALS->getDatabaseViewName(OPL::DatabaseView::Airports));
    m_model->select();

    // Fetch all rows so maps can be populated correctly
    while (m_model->canFetchMore())
        m_model->fetchMore();

    refreshIndices();

    connect(DB, &OPL::Database::dataBaseUpdated, this, [this](OPL::DbTable table) {
        if (table == OPL::DbTable::v2Airports || table == OPL::DbTable::v2AirportCodes) {
            LOG << "Updating Airport Info.";
            m_model->select();
            while (m_model->canFetchMore())
                m_model->fetchMore();
            refreshIndices();
        }
    });
}

void AirportInfo::refreshIndices()
{
    int row_count = m_model->rowCount();
    m_airportIdToRow.clear();
    m_icaoToAirportId.clear();
    m_iataToAirportId.clear();
    m_allCodesToAirportId.clear();
    m_icaoToAirportId.reserve(row_count);
    m_iataToAirportId.reserve(row_count);
    m_airportIdToRow.reserve(row_count);

    for (int row = 0; row < row_count; ++row) {
        int airportId = m_model->data(m_model->index(row, COLUMN_AIRPORT_ID)).toInt();
        QString icao  = m_model->data(m_model->index(row, COLUMN_ICAO)).toString();
        QString iata  = m_model->data(m_model->index(row, COLUMN_IATA)).toString();

        m_airportIdToRow.insert(airportId, row);
        if (!icao.isEmpty()){
            m_icaoToAirportId.insert(icao, airportId);
            m_allCodesToAirportId.insert(icao, airportId);
        }
        if (!iata.isEmpty()) {
            m_iataToAirportId.insert(iata, airportId);
            m_allCodesToAirportId.insert(iata, airportId);
        }
    }
}

QString AirportInfo::dataForAirportId(int airportId, int column) const
{
    int row = m_airportIdToRow.value(airportId, 0);
    return m_model->data(m_model->index(row, column)).toString();
}

// Getters

QString AirportInfo::nameFromRowId(int airport_id)
{
    return dataForAirportId(airport_id, COLUMN_NAMES);
}

QString AirportInfo::nameFromIcao(const QString &icao)
{
    return dataForAirportId(m_icaoToAirportId.value(icao, 0), COLUMN_NAMES);
}

QString AirportInfo::nameFromIata(const QString &iata)
{
    return dataForAirportId(m_iataToAirportId.value(iata, 0), COLUMN_NAMES);
}

int AirportInfo::idFromIcao(const QString &icao) { return m_icaoToAirportId.value(icao, 0); }

int AirportInfo::idFromIata(const QString &iata) { return m_iataToAirportId.value(iata, 0); }

QString AirportInfo::icao(int airport_id) { return dataForAirportId(airport_id, COLUMN_ICAO); }

QString AirportInfo::icao(const QString &iata)
{
    return dataForAirportId(m_iataToAirportId.value(iata, 0), COLUMN_ICAO);
}

QString AirportInfo::iata(int airport_id) { return dataForAirportId(airport_id, COLUMN_IATA); }

QString AirportInfo::iata(const QString &icao)
{
    return dataForAirportId(m_icaoToAirportId.value(icao, 0), COLUMN_IATA);
}
bool AirportInfo::exists(int airport_id) const { return m_airportIdToRow.contains(airport_id); }
