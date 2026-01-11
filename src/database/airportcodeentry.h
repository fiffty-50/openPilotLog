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

    /*!
     * \brief return the airports IATA code (3-letter)
     */
    const QString getIataCode() const;

    /*!
     * \brief return the airports ICAO code (4-letter)
     */
    const QString getIcaoCode() const;

    /*!
     * \brief return a custom airport code, if set
     */
    const QString getCustomCode() const;

    enum class CodeType { ICAO, IATA, OTHER };

    bool setAirportCode(CodeType type, const QDate &validFrom, const QDate &validTo);

private:
    const static inline QString ROWID = QStringLiteral("airport_code_id");
    const static inline QString AIRPORTID = QStringLiteral("airport_id");
    const static inline QString CODE_TYPE = QStringLiteral("code_type");
    const static inline QString VALID_FROM = QStringLiteral("valid_from_jd");
    const static inline QString VALID_TO = QStringLiteral("valid_to_jd");

};

} // namespace OPL

#endif // AIRPORTCODEENTRY_H
