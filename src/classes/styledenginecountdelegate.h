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
#ifndef STYLEDENGINECOUNTDELEGATE_H
#define STYLEDENGINECOUNTDELEGATE_H

#include <QStyledItemDelegate>

/*!
 * \brief A styled delegate to convert a boolean engine count into a user-facing string.
 * \details The database stores engine count as a boolean value: single-engine (0) or multi-engine (1).
 * This delegate converts that boolean into a human-readable string for display.
 */
class StyledEngineCountDelegate : public QStyledItemDelegate
{
public:
    explicit StyledEngineCountDelegate(QObject *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // STYLEDENGINECOUNTDELEGATE_H
