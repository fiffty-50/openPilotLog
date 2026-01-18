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
#include "styledtimedelegate.h"
#include "src/classes/time.h"

StyledTimeDelegate::StyledTimeDelegate(const OPL::DateTimeFormat &format, QObject *parent)
    : QStyledItemDelegate{parent}, m_format(format)
{
}

QString StyledTimeDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    const OPL::Time time(value.toInt(), m_format);
    return time.toString();
}
