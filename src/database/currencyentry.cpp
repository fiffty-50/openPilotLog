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

namespace OPL {

CurrencyEntry::CurrencyEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Currencies, row_id, row_data)
{}

const QString CurrencyEntry::getTableName() const
{
    return TABLE_NAME;
}

void CurrencyEntry::setName(const QString &displayName)
{
    auto data = getData();
    data.insert(NAME, displayName);
    setData(data);
}

const QString CurrencyEntry::getName() const
{
    return getData().value(NAME).toString();
}

void CurrencyEntry::setExpiryDate(const Date &date)
{
    auto data = getData();
    data.insert(EXPIRYDATE, date.toJulianDay());
    setData(data);
}

const Date CurrencyEntry::getExpiryDate(const OPL::DateTimeFormat &format) const
{
    return OPL::Date(getData().value(EXPIRYDATE).toInt(), format);
}

} // namespace OPL
