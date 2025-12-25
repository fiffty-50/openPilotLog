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
 * \brief A Row representing a Flight entry
 * \details A FlightEntry represents a row in the flights table of the database and extends OPL::Row
 * It contains mandatory basic information like Departure, Destination and block times
 * but can also contain a number of additional items.
 *
 * The FlightEntryParser class provides convenience functions to bridge user input to database format,
 * so direct interaction with this class should not be required for most ui-facing tasks.
 */
class FlightEntry : public Row
{
public:
    FlightEntry();
    FlightEntry(const RowData_T &row_data);
    FlightEntry(int row_id, const RowData_T &row_data);

    /*!
     * \brief return a QStringList of invalid mandatory fields.
     *
     * \details The list is used in the GUI, so it contains user-facing translatable display names
     * instead of the database field names
     */
    QStringList invalidFields() const;

    const static QList<QString> *getMandatoryFields() { return &mandatoryFields; }

    /*!
     * \brief returns the name of the flights table in the database
     */
    const QString getTableName() const override;

    /*!
     * \brief returns a String representation of the key data of this flight
     */
    QString getFlightSummary() const;


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

private:
    static constexpr int NEW_ENTRY = 0;
    static const inline QString TABLE_NAME = QStringLiteral("flights");
    static inline const QList<QString> mandatoryFields = {
        FlightEntry::DOFT,
        FlightEntry::DEPT,
        FlightEntry::DEST,
        FlightEntry::TOFB,
        FlightEntry::TONB,
        FlightEntry::TBLK,
        FlightEntry::PIC,
        FlightEntry::ACFT
    };
    static inline const QList<QString> allFields = {
        ROWID          ,
        DOFT           ,
        DEPT           ,
        DEST           ,
        TOFB           ,
        TONB           ,
        PIC            ,
        ACFT           ,
        TBLK           ,
        TSPSE          ,
        TSPME          ,
        TMP            ,
        TNIGHT         ,
        TIFR           ,
        TPIC           ,
        TPICUS         ,
        TSIC           ,
        TDUAL          ,
        TFI            ,
        TSIM           ,
        PILOTFLYING    ,
        TODAY          ,
        TONIGHT        ,
        LDGDAY         ,
        LDGNIGHT       ,
        AUTOLAND       ,
        SECONDPILOT    ,
        THIRDPILOT     ,
        APPROACHTYPE   ,
        FLIGHTNUMBER   ,
        REMARKS        ,
    };
};
} // namespace OPL
#endif // FLIGHTENTRY_H
