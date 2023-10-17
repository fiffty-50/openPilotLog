/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#ifndef AIRPORTENTRY_H
#define AIRPORTENTRY_H
#include "src/database/row.h"

namespace OPL {

/*!
 * \brief A Row representing an Airport entry. See Row class for details.
 */
class AirportEntry : public Row
{
    const static inline QString TABLE_NAME = QStringLiteral("airports");
public:
    AirportEntry();
    AirportEntry(const RowData_T &row_data);
    AirportEntry(int row_id, const RowData_T &row_data);
    const QString getTableName() const override;

    /*!
     * \brief return the airports IATA code (3-letter)
     */
    const QString getIataCode() const;

    /*!
     * \brief return the airports ICAO code (4-letter)
     */
    const QString getIcaoCode() const;

    /*!
     * \brief The ICAO code is a 4-letter alphanumeric identifier for airports
     */
    const static inline QString ICAO           = QStringLiteral("icao");
    /*!
     * \brief The IATA code is a 3-letter alphanumeric identifier for airports
     */
    const static inline QString IATA           = QStringLiteral("iata");
    /*!
     * \brief The airports common name
     */
    const static inline QString NAME           = QStringLiteral("name");
    /*!
     * \brief The airports latitude. Stored as a double
     */
    const static inline QString LAT            = QStringLiteral("lat");

    /*!
     * \brief The airports longitude. Stored as a double
     */
    const static inline QString LON            = QStringLiteral("long");

    /*!
     * \brief The country the airport is located in
     */
    const static inline QString COUNTRY        = QStringLiteral("country");
    /*!
     * \brief The altitude aboe mea sea level the airport is located at.
     */
    const static inline QString ALTITIDUE      = QStringLiteral("alt");

    /*!
     * \brief The airports timezone Offset from UTC
     */
    const static inline QString UTC_OFFSET     = QStringLiteral("utcoffset");
    /*!
     * \brief The timezone (Olson classification) the airport is situated in
     */
    const static inline QString TZ_OLSON       = QStringLiteral("tzolson");
};

} // namespace OPL

#endif // AIRPORTENTRY_H
