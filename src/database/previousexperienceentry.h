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
#ifndef PREVIOUSEXPERIENCEENTRY_H
#define PREVIOUSEXPERIENCEENTRY_H
#include "src/database/flightentry.h"
#include "src/database/row.h"

namespace OPL {

/*!
 * \brief A Row representing an Airport entry. See Row class for details.
 */
class PreviousExperienceEntry : public Row
{
    const static inline QString TABLE_NAME = QStringLiteral("previousExperience");
public:
    PreviousExperienceEntry();
    PreviousExperienceEntry(const RowData_T &row_data);
    PreviousExperienceEntry(int row_id, const RowData_T &row_data);
    const QString getTableName() const override;

    // these literals already exist in the FlightEntry class, so we can just copy them
    static const inline auto TBLK           = OPL::FlightEntry::TBLK;
    static const inline auto TSPSE          = OPL::FlightEntry::TSPSE;
    static const inline auto TSPME          = OPL::FlightEntry::TSPME;
    static const inline auto TMP            = OPL::FlightEntry::TMP;
    static const inline auto TNIGHT         = OPL::FlightEntry::TNIGHT;
    static const inline auto TIFR           = OPL::FlightEntry::TIFR;
    static const inline auto TPIC           = OPL::FlightEntry::TPIC;
    static const inline auto TPICUS         = OPL::FlightEntry::TPICUS;
    static const inline auto TSIC           = OPL::FlightEntry::TSIC;
    static const inline auto TDUAL          = OPL::FlightEntry::TDUAL;
    static const inline auto TFI            = OPL::FlightEntry::TFI;
    static const inline auto TSIM           = OPL::FlightEntry::TSIM;
    static const inline auto TODAY          = OPL::FlightEntry::TODAY;
    static const inline auto TONIGHT        = OPL::FlightEntry::TONIGHT;
    static const inline auto LDGDAY         = OPL::FlightEntry::LDGDAY;
    static const inline auto LDGNIGHT       = OPL::FlightEntry::LDGNIGHT;
};

} // namespace OPL
#endif // PREVIOUSEXPERIENCEENTRY_H
