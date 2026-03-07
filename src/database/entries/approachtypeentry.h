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
#ifndef APPROACHTYPEENTRY_H
#define APPROACHTYPEENTRY_H

#include "src/database/entries/row.h"
#include "src/opl.h"
#include <QObject>

namespace OPL {

/*!
 * \brief A Row representing an Approach Type Entry
 * \details
 * The approach_types table holds the various approach types a user can select.
 * This table is thin and only holds a row_id and the approach type name.
 */
class ApproachTypeEntry : public OPL::Row {
  public:
    /*!
     * \brief Create a new Approach Type entry
     */
    ApproachTypeEntry();

    /*!
     * \brief edit an existing Approach Type entry
     */
    ApproachTypeEntry(int row_id, const OPL::RowData_T &row_data);

    /*!
     * \brief true if it contains a non-null name
     */
    bool isValid() const override;

    /*!
     * \brief set a new Approach Name
     */
    bool setName(const QString &name);

  private:
    const static inline QString TYPE_NAME = QStringLiteral("approach_type");

    const static inline QStringList FIELDS = {
        TYPE_NAME,
    };
};

} // namespace OPL

#endif // APPROACHTYPEENTRY_H
