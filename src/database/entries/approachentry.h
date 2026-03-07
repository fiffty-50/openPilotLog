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
#ifndef APPROACHENTRY_H
#define APPROACHENTRY_H

#include "src/database/entries/row.h"

namespace OPL {

/*!
 * \brief The ApproachEntry class represents a Row in the approach_events table
 * \details Since more than one approach can be logged per flight and they do
 * not necessarily result in a movement event (a landing), approaches are kept
 * seperate.
 *
 * There are two constructors available. Use
 * - ApproachEntry(int event_id, const QString &app_type)
 * for creating a new ApproachEntry for an associated log_event
 * - ApproachEntry(int row_id, const RowData &row_data)
 * when retreiving values from the datbase.
 *
 * \note The constructor for values based on database values takes the row_id (Primary Key)
 * while the constructor for creating a new entry takes the associated event_id (Foreign Key).
 */
class ApproachEntry : public OPL::Row {
  public:
    ApproachEntry() = delete;

    /*!
     * \brief Create a new approach entry for an associated event_id
     */
    ApproachEntry(int event_id, int approach_type_id);

    /*!
     * \brief Create an approach entry based on databasa values
     */
    ApproachEntry(int row_id, const RowData_T &row_data);

    bool isValid() const override;

    bool setEventId(int event_id);
    bool setApproachType(int approach_id);

    int getEventId() const { return m_rowData.value(EVENT_ID).toInt(); }
    int getApproachId() const { return m_rowData.value(APP_TYPE).toInt(); }

  private:
    const static inline QString EVENT_ID   = QStringLiteral("event_id");
    const static inline QString APP_TYPE   = QStringLiteral("approach_type");
    const static inline QStringList FIELDS = {EVENT_ID, APP_TYPE};
};

} // namespace OPL
#endif // APPROACHENTRY_H
