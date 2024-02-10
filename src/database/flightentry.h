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
#ifndef FLIGHTENTRY_H
#define FLIGHTENTRY_H
#include "src/database/row.h"


namespace OPL {
/*!
 * \brief A Row representing a Flight entry. See Row class for details.
 */
class FlightEntry : public Row
{
    const static inline QString TABLE_NAME = QStringLiteral("flights");
public:
    FlightEntry();
    FlightEntry(const RowData_T &row_data);
    FlightEntry(int row_id, const RowData_T &row_data);

    const QString getTableName() const override;

    /*!
     * \brief returns a String representation of the key data of this flight
     */
    const QString getFlightSummary() const;

    const static inline QString ROWID          = QStringLiteral("flight_id");
    const static inline QString DOFT           = QStringLiteral("doft");
    const static inline QString DEPT           = QStringLiteral("dept");
    const static inline QString DEST           = QStringLiteral("dest");
    const static inline QString TOFB           = QStringLiteral("tofb");
    const static inline QString TONB           = QStringLiteral("tonb");
    const static inline QString PIC            = QStringLiteral("pic");
    const static inline QString ACFT           = QStringLiteral("acft");
    const static inline QString TBLK           = QStringLiteral("tblk");
    const static inline QString TSPSE          = QStringLiteral("tSPSE");
    const static inline QString TSPME          = QStringLiteral("tSPME");
    const static inline QString TMP            = QStringLiteral("tMP");
    const static inline QString TNIGHT         = QStringLiteral("tNIGHT");
    const static inline QString TIFR           = QStringLiteral("tIFR");
    const static inline QString TPIC           = QStringLiteral("tPIC");
    const static inline QString TPICUS         = QStringLiteral("tPICUS");
    const static inline QString TSIC           = QStringLiteral("tSIC");
    const static inline QString TDUAL          = QStringLiteral("tDUAL");
    const static inline QString TFI            = QStringLiteral("tFI");
    const static inline QString TSIM           = QStringLiteral("tSIM");
    const static inline QString PILOTFLYING    = QStringLiteral("pilotFlying");
    const static inline QString TODAY          = QStringLiteral("toDay");
    const static inline QString TONIGHT        = QStringLiteral("toNight");
    const static inline QString LDGDAY         = QStringLiteral("ldgDay");
    const static inline QString LDGNIGHT       = QStringLiteral("ldgNight");
    const static inline QString AUTOLAND       = QStringLiteral("autoland");
    const static inline QString SECONDPILOT    = QStringLiteral("secondPilot");
    const static inline QString THIRDPILOT     = QStringLiteral("thirdPilot");
    const static inline QString APPROACHTYPE   = QStringLiteral("approachType");
    const static inline QString FLIGHTNUMBER   = QStringLiteral("flightNumber");
    const static inline QString REMARKS        = QStringLiteral("remarks");
};
} // namespace OPL
#endif // FLIGHTENTRY_H
