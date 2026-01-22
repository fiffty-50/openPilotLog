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
#ifndef MOVEMENTENTRY_H
#define MOVEMENTENTRY_H

#include "row.h"

namespace OPL {

class MovementEntry : public OPL::Row {
  public:
    MovementEntry(int event_id, int airport_id = 0, bool is_landing = true, bool is_night = false,
                  bool is_autoland = false);
    MovementEntry(int row_id, const OPL::RowData_T &row_data);

    bool isValid() const override;

    bool isLanding() const;
    bool isNight() const;
    bool isAutoland() const;
    int airportId() const;

  private:
    const static inline QString TABLE_NAME  = QStringLiteral("movement_events");
    const static inline QString EVENT_ID    = QStringLiteral("event_id");
    const static inline QString AIRPORT_ID  = QStringLiteral("airport_id");
    const static inline QString IS_LANDING  = QStringLiteral("is_landing");
    const static inline QString IS_NIGHT    = QStringLiteral("is_night");
    const static inline QString IS_AUTOLAND = QStringLiteral("is_autoland");

    const static inline QStringList FIELDS = {
        EVENT_ID, AIRPORT_ID, IS_LANDING, IS_NIGHT, IS_AUTOLAND,
    };
};

} // namespace OPL

#endif // MOVEMENTENTRY_H
