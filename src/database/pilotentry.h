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
#ifndef PILOTENTRY_H
#define PILOTENTRY_H
#include "src/database/row.h"

namespace OPL {

/*!
 * \brief A Row representing a Pilot entry. See Row class for details.
 */
class PilotEntry : public Row
{
    const static inline QString TABLE_NAME = QStringLiteral("pilots");
public:
    PilotEntry();
    PilotEntry(const RowData_T &row_data);
    PilotEntry(int row_id, const RowData_T &row_data);
    const QString getTableName() const override;

    /*!
     * \brief Return the pilots last name
     */
    const QString getLastName()  const;

    /*!
     * \brief Return the pilots first name
     */
    const QString getFirstName() const;

    const static inline QString ROWID           = QStringLiteral("pilot_id");
    /*!
     * \brief The sql column name for the pilots last name
     */
    const static inline QString LASTNAME        = QStringLiteral("lastname");
    /*!
     * \brief The sql column name for the pilots first name
     */
    const static inline QString FIRSTNAME       = QStringLiteral("firstname");

    /*!
     * \brief The sql column name for an alias for the pilot
     */
    const static inline QString ALIAS           = QStringLiteral("alias");
    /*!
     * \brief The company the pilots works for
     */
    const static inline QString COMPANY         = QStringLiteral("company");
    /*!
     * \brief The sql column name for the pilots employee number
     */
    const static inline QString EMPLOYEEID      = QStringLiteral("employeeid");
    /*!
     * \brief The sql column name for a pilots phone number
     */
    const static inline QString PHONE           = QStringLiteral("phone");
    /*!
     * \brief the sql column name for a pilots email address
     */
    const static inline QString EMAIL           = QStringLiteral("email");
};

} // namespace OPL

#endif // PILOTENTRY_H
