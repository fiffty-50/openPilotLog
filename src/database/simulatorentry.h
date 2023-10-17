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
#ifndef SIMULATORENTRY_H
#define SIMULATORENTRY_H
#include "src/database/row.h"

namespace OPL {

/*!
 * \brief A Row representing a Simulator entry. See Row class for details.
 */
class SimulatorEntry : public Row
{
    const static inline QString TABLE_NAME = QStringLiteral("simulators");
public:
    SimulatorEntry();
    SimulatorEntry(const RowData_T &row_data);
    SimulatorEntry(int row_id, const RowData_T &row_data);

    const QString getTableName() const override;

    /*!
     * \brief The sql column name for the row id
     */
    const static inline QString  ROWID       = QStringLiteral("session_id");
    /*!
     * \brief The sql column name for the date
     */
    const static inline QString  DATE        = QStringLiteral("date");
    /*!
     * \brief The sql column name for the total time
     */
    const static inline QString  TIME        = QStringLiteral("totalTime");
    /*!
     * \brief The sql column name for the simulator type (FNPT/FFS)
     */
    const static inline QString  TYPE        = QStringLiteral("deviceType");
    /*!
     * \brief The sql column name for the simulators aircraft type
     */
    const static inline QString  ACFT        = QStringLiteral("aircraftType");
    /*!
     * \brief The sql column name for the simulators registration
     */
    const static inline QString  REG         = QStringLiteral("registration");
    /*!
     * \brief The sql column name for remarks
     */
    const static inline QString  REMARKS     = QStringLiteral("remarks");
};

} // namespace OPL

#endif // SIMULATORENTRY_H
