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

#ifndef LOGENTRY_H
#define LOGENTRY_H

#include "row.h"
#include <qobject.h>

namespace OPL {

class LogEntry : public OPL::Row {
  public:
    explicit LogEntry();
    explicit LogEntry(int rowId, const RowData_T &rowData);

    bool isValid() const override;

    enum EventType { Flight, Sim };
    static std::optional<EventType> fromString(const QString &eventString);
    static std::optional<QString> toString(EventType eventType);

    bool setEventType(const QString &eventType);
    QString getEventType() const;

    bool setDate(const QDate &date);
    QDate getDate() const;

  protected:
    const static inline QString DATE          = QStringLiteral("event_date_jd");
    const static inline QString EVENT_TYPE    = QStringLiteral("event_type");
    const static inline QString REMARKS       = QStringLiteral("remarks");
    const static inline QList<QString> FIELDS = {
        DATE,
        EVENT_TYPE,
        REMARKS,
    };

  private:
    const static inline QString EVENT_TYPE_FLIGHT = QStringLiteral("FLT");
    const static inline QString EVENT_TYPE_SIM    = QStringLiteral("SIM");
};

} // namespace OPL

#endif // LOGENTRY_H
