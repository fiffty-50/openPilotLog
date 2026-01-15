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
#ifndef AIRPORTCODEENTRY_H
#define AIRPORTCODEENTRY_H

#include "row.h"

namespace OPL {

class AirportCodeEntry : public OPL::Row
{
public:
    AirportCodeEntry();
    AirportCodeEntry(const RowData_T &rowData);
    AirportCodeEntry(int rowId, const RowData_T &row_data);

    QString getTableName() const { return OPL::GLOBALS->getDbTableName(m_table); }
    bool isValid() const override;

    enum class CodeType { ICAO, IATA, OTHER };

    bool setAirportCode(CodeType type, const QDate &validFrom, const QDate &validTo);
    std::tuple<CodeType, QDate, QDate> getAirportCode() const;
    CodeType getCodeType() const;
    QDate getValidFromDate() const;
    QDate getValidToDate() const;

    /*!
     * \brief Get the specified code for a given rowId
     * \param type - The Type of Code, see AirportCodeEntry::CodeType
     * \param airport_id - The airport_id of the airport that is being queried.
     * \return The Current Airport Code
     */
    static QString getCurrentCode(int airport_id, CodeType type);

private:
    const static inline QString ROWID = QStringLiteral("airport_code_id");
    const static inline QString AIRPORTID = QStringLiteral("airport_id");
    const static inline QString CODE_TYPE = QStringLiteral("code_type");
    const static inline QString VALID_FROM = QStringLiteral("valid_from_jd");
    const static inline QString VALID_TO = QStringLiteral("valid_to_jd");

    const static inline QList<QString> FIELDS = {
        AIRPORTID, CODE_TYPE, VALID_FROM, VALID_TO
    };

    const static inline QHash<CodeType, QString> CODE_TYPES_MAP = {
        {CodeType::ICAO, QStringLiteral("ICAO")},
        {CodeType::IATA, QStringLiteral("IATA")},
        {CodeType::OTHER, QStringLiteral("OTHER")},
    };
};

} // namespace OPL

#endif // AIRPORTCODEENTRY_H
