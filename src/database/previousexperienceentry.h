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
    const static inline QString TBLK           = OPL::FlightEntry::TBLK;
    const static inline QString TSPSE          = OPL::FlightEntry::TSPSE;
    const static inline QString TSPME          = OPL::FlightEntry::TSPME;
    const static inline QString TMP            = OPL::FlightEntry::TMP;
    const static inline QString TNIGHT         = OPL::FlightEntry::TNIGHT;
    const static inline QString TIFR           = OPL::FlightEntry::TIFR;
    const static inline QString TPIC           = OPL::FlightEntry::TPIC;
    const static inline QString TPICUS         = OPL::FlightEntry::TPICUS;
    const static inline QString TSIC           = OPL::FlightEntry::TSIC;
    const static inline QString TDUAL          = OPL::FlightEntry::TDUAL;
    const static inline QString TFI            = OPL::FlightEntry::TFI;
    const static inline QString TSIM           = OPL::FlightEntry::TSIM;
    const static inline QString TODAY          = OPL::FlightEntry::TODAY;
    const static inline QString TONIGHT        = OPL::FlightEntry::TONIGHT;
    const static inline QString LDGDAY         = OPL::FlightEntry::LDGDAY;
    const static inline QString LDGNIGHT       = OPL::FlightEntry::LDGNIGHT;
};

} // namespace OPL
#endif // PREVIOUSEXPERIENCEENTRY_H
