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
#ifndef AIRPORTENTRY_H
#define AIRPORTENTRY_H
#include "src/database/row.h"

namespace OPL {

/*!
 * \brief A Row representing an Airport entry. See Row class for details.
 */
class AirportEntry : public Row {
    const static inline QString TABLE_NAME = QStringLiteral("airports");

  public:
    AirportEntry();
    AirportEntry(const RowData_T &row_data);
    AirportEntry(int row_id, const RowData_T &row_data);

    bool isValid() const override;

    bool setAirportName(const QString &input);
    bool setTimezone(const QString &input);
    bool setLatitude(double input);
    bool setLongitude(double input);

    QString getAirportName() const;
    QString getTimezone() const;
    double getLatitude() const;
    double getLongitude() const;

  private:
    const static inline QString ROWID = QStringLiteral("airport_id");
    /*!
     * \brief The airports common name
     */
    const static inline QString NAME = QStringLiteral("airport_name");
    /*!
     * \brief The airports latitude. Stored as a double
     */
    const static inline QString LATITUDE = QStringLiteral("latitude");

    /*!
     * \brief The airports longitude. Stored as a double
     */
    const static inline QString LONGITUDE = QStringLiteral("longitude");

    /*!
     * \brief The timezone (Olson classification) the airport is situated in
     */
    const static inline QString TZ_OLSON = QStringLiteral("timezone_olson");

    const static inline QList<QString> FIELDS = {NAME, LATITUDE, LONGITUDE, TZ_OLSON};
};

} // namespace OPL

#endif // AIRPORTENTRY_H
