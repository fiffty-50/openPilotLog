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

#include "logentry.h"
#include "src/classes/date.h"
#include "src/opl.h"
#include <optional>

namespace OPL {

LogEntry::LogEntry() : Row(DbTable::v2LogEvents, &FIELDS) {}
LogEntry::LogEntry(int rowId, const RowData_T &rowData)
    : Row(DbTable::v2LogEvents, rowId, rowData, &FIELDS)
{
}

bool LogEntry::isValid() const
{

    bool allValid = true;

    // Check if values are present
    allValid &= m_rowData.contains(DATE);
    allValid &= m_rowData.contains(EVENT_TYPE);

    if (!allValid) {
        return false;
    }

    // Check date is within range
    int date = m_rowData.value(DATE).toInt();
    allValid &= OPL::Date::julianDayIsValid(date);

    // Check event_type is valid
    allValid &= fromString(m_rowData.value(EVENT_TYPE).toString()) != std::nullopt;

    return allValid;
}

std::optional<LogEntry::EventType> LogEntry::fromString(const QString &eventString)
{
    if (eventString == EVENT_TYPE_FLIGHT) {
        return EventType::Flight;
    }
    if (eventString == EVENT_TYPE_SIM) {
        return EventType::Sim;
    }
    return std::nullopt;
}

std::optional<QString> LogEntry::toString(LogEntry::EventType eventType)
{
    switch (eventType) {
    case EventType::Flight:
        return EVENT_TYPE_FLIGHT;
        break;
    case EventType::Sim:
        return EVENT_TYPE_SIM;
        break;
    default:
        return std::nullopt;
    }
}

// Setters
bool LogEntry::setEventType(const QString &eventString)
{
    std::optional<EventType> eventType = fromString(eventString);
    if (!eventType) {
        return false;
    }

    m_rowData.insert(EVENT_TYPE, eventString);
    return true;
}

bool LogEntry::setDate(const QDate &date)
{
    const int date_jd = date.toJulianDay();
    if (!OPL::Date::julianDayIsValid(date_jd)) {
        return false;
    }

    m_rowData.insert(DATE, date_jd);
    return true;
}

bool LogEntry::setDate(int date_jd) {
    if (!OPL::Date::julianDayIsValid(date_jd)) {
        return false;
    }

    m_rowData.insert(DATE, date_jd);
    return true;
}

bool LogEntry::setRemarks(const QString &remarks)
{
    m_rowData.insert(REMARKS, remarks);
    return true;
}

// Getters
QString LogEntry::getEventType() const { return m_rowData.value(EVENT_TYPE).toString(); }

QDate LogEntry::getDate() const { return QDate::fromJulianDay(m_rowData.value(DATE).toInt()); }

QString LogEntry::getRemarks() const { return m_rowData.value(REMARKS).toString(); }

} // namespace OPL
