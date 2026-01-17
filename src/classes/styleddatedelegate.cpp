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
#include "styleddatedelegate.h"
#include "src/classes/date.h"

StyledDateDelegate::StyledDateDelegate(const OPL::DateTimeFormat &dateFormat, QObject *parent)
    :
    QStyledItemDelegate(parent),
    m_format(dateFormat)
{}

QString StyledDateDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    return OPL::Date(value.toInt(), m_format).toString();
}
