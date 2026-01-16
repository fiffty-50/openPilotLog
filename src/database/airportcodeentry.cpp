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
#include "src/classes/date.h"
#include <QSqlQuery>

namespace OPL {

AirportCodeEntry::AirportCodeEntry()
    : Row(DbTable::v2AirportCodes
    , &FIELDS)
{}

AirportCodeEntry::AirportCodeEntry(int rowId, const RowData_T &rowData)
    : Row(DbTable::v2AirportCodes
    , rowId
    , rowData
    , &FIELDS)
{}

bool AirportCodeEntry::isValid() const
{
    bool isValid = true;
    isValid &= CODE_TYPES_MAP.values().contains(m_rowData.value(CODE_TYPE));

    int validFrom = m_rowData.value(VALID_FROM).toInt();
    isValid &= validFrom < OPL::Date::getMinimumDateJulianDay();
    isValid &= validFrom > OPL::Date::getMaximumDateJulianDay();

    return isValid;
}

bool AirportCodeEntry::setAirportCode(CodeType type, const QDate &validFrom, const QDate &validTo)
{
    if(validFrom < OPL::Date::getMinimumDate() || validFrom > OPL::Date::getMaximumDate())
        return false;

    if(validTo > OPL::Date::getMaximumDate())
        return false;
    // QDate comparison operator is defined such that an invalid QDate is less than any
    // valid date for invalid QDate, set valid to date to NULL
    if(validTo < OPL::Date::getMinimumDate() || validTo == OPL::Date::getMaximumDate()) {
        m_rowData.insert(VALID_TO, QVariant(QMetaType(QMetaType::QString)));
    } else {
        m_rowData.insert(VALID_TO, validTo.toJulianDay());
    }

    m_rowData.insert(VALID_FROM, validFrom.toJulianDay());
    m_rowData.insert(CODE_TYPE, CODE_TYPES_MAP.value(type));
    return true;
}

QDate AirportCodeEntry::getValidFromDate() const
{
    int validFromJd = m_rowData.value(VALID_FROM).toInt();
    if(validFromJd < OPL::Date::getMinimumDateJulianDay()
        || validFromJd > OPL::Date::getMaximumDateJulianDay()) {
        return QDate();
    }
    return QDate::fromJulianDay(validFromJd);
}

QDate AirportCodeEntry::getValidToDate() const
{
    int validToJd = m_rowData.value(VALID_FROM).toInt();
    if(validToJd < OPL::Date::getMinimumDateJulianDay()
        || validToJd > OPL::Date::getMaximumDateJulianDay()) {
        return QDate();
    }
    return QDate::fromJulianDay(validToJd);
}

AirportCodeEntry::CodeType AirportCodeEntry::getCodeType() const
{
    return CODE_TYPES_MAP.key(m_rowData.value(CODE_TYPE).toString());
}

std::tuple<AirportCodeEntry::CodeType, QDate, QDate> AirportCodeEntry::getAirportCode() const
{
    CodeType type = CODE_TYPES_MAP.key(m_rowData.value(CODE_TYPE).toString());

    const QDate valid_from = QDate::fromJulianDay(m_rowData.value(VALID_FROM).toInt());
    const QDate valid_to = QDate::fromJulianDay(m_rowData.value(VALID_TO).toInt());

    return {type, valid_from, valid_to};
}

QString AirportCodeEntry::getCurrentCode(int airport_id, CodeType type)
{
    QString query = QStringLiteral("WITH CurrentCode AS ( "
    "     SELECT"
    "        airport_id,"
    "        airport_code,"
    "        valid_from_jd,"
    "        valid_to_jd,"
    "        ROW_NUMBER() OVER ("
    "            PARTITION BY airport_id"
    "            ORDER BY valid_from_jd DESC"
    "        ) AS row_num"
    "    FROM airport_codes"
    "    WHERE airport_id = ?"
    "      AND code_type = ?"
    "      AND (valid_to_jd IS NULL OR valid_to_jd >= julianday('now'))"
    "      AND valid_from_jd <= julianday('now')"
    ") "
    "SELECT "
    "    airport_code "
    "FROM CurrentCode "
    "WHERE row_num = 1 ");
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
        LOG << QStringLiteral("No airport code found for airport_id: ") << airport_id;
        return {};
    }

    return q.value(0).toString();
}


} // namespace OPL
