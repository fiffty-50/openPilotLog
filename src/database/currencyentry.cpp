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
#include "currencyentry.h"
#include "src/database/database.h"

namespace OPL {

//CurrencyEntry::CurrencyEntry()
//    : Row(DbTable::Currencies, 0)
//{}

//CurrencyEntry::CurrencyEntry(const RowData_T &row_data)
//    : Row(DbTable::Currencies, 0, row_data)
//{}

CurrencyEntry::CurrencyEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Currencies, row_id, row_data)
{}

const QString CurrencyEntry::getTableName() const
{
    return TABLE_NAME;
}

void CurrencyEntry::setDisplayName(const QString &displayName)
{
    auto data = getData();
    data.insert(DISPLAY_NAME, displayName);
    setData(data);
    DB->commit(*this);
}

const QString CurrencyEntry::getDisplayName() const
{
    return getData().value(DISPLAY_NAME).toString();
}

void CurrencyEntry::setExpiryDate(const QDate &date)
{
    auto data = getData();
    data.insert(EXPIRYDATE, date.toString(Qt::ISODate));
    setData(data);
    DB->commit(*this);
}

const QDate CurrencyEntry::getExpiryDate() const
{
    const QString &date = getData().value(EXPIRYDATE).toString();
    return QDate::fromString(date, Qt::ISODate);
}

} // namespace OPL
