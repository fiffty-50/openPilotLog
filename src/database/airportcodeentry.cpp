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
#include "airportcodeentry.h"
#include <QSqlQuery>

namespace OPL {

AirportCodeEntry::AirportCodeEntry()
    : Row(DbTable::v2AirportCodes, 0){}

AirportCodeEntry::AirportCodeEntry(const RowData_T &rowData)
    : Row(DbTable::v2AirportCodes, 0, rowData){}

AirportCodeEntry::AirportCodeEntry(int rowId, const RowData_T &rowData)
    : Row(DbTable::v2AirportCodes, rowId, rowData){}

const QString AirportCodeEntry::getIataCode() const
{
    Q_UNIMPLEMENTED();
    return QString();
}

const QString AirportCodeEntry::getIcaoCode() const
{
    Q_UNIMPLEMENTED();
    return QString();
}

const QString AirportCodeEntry::getCustomCode() const
{
    Q_UNIMPLEMENTED();
    return QString();
}

bool AirportCodeEntry::setAirportCode(CodeType type, const QDate &validFrom, const QDate &validTo)
{
    Q_UNIMPLEMENTED();
    return false;
}

QString AirportCodeEntry::getCurrentCode(int airport_id, CodeType type)
{
    QString query = R"(
    WITH CurrentCode AS (
        SELECT
            airport_id,
            airport_code,
            valid_from_jd,
            valid_to_jd,
            ROW_NUMBER() OVER (
                PARTITION BY airport_id
                ORDER BY valid_from_jd DESC
            ) AS row_num
        FROM airport_codes
        WHERE airport_id = ?
          AND code_type = ?
          AND (valid_to_jd IS NULL OR valid_to_jd >= julianday('now'))
          AND valid_from_jd <= julianday('now')
    )
    SELECT
        airport_code
    FROM CurrentCode
    WHERE row_num = 1;
    )";
    QSqlQuery q(query);
    q.addBindValue(airport_id);

    switch (type) {
    case CodeType::ICAO:
        q.addBindValue(QStringLiteral("ICAO"));
        break;
    case CodeType::IATA:
        q.addBindValue(QStringLiteral("IATA"));
        break;
    case CodeType::OTHER:
        q.addBindValue(QStringLiteral("OTHER"));
        break;
    default:
        break;
    }

    if( !q.exec() || ! q.next()) {
        return {};
    }

    return q.value(0).toString();
}


} // namespace OPL
