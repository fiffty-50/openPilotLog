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
#ifndef STYLEDDATEDELEGATE_H
#define STYLEDDATEDELEGATE_H

#include <QStyledItemDelegate>

/*!
 * \brief The StyledDateDelegate class is used to display a database date value human-readable.
 * \details The database stores dates as an integer representing the days elapsed since the
 * beginning of the julian calendar. This integer has to be converted to a human-readable date
 * according to the users selected date format.
 */
class StyledDateDelegate : public QStyledItemDelegate {
  public:
    StyledDateDelegate(const QString &date_format, QObject *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;

  private:
    const QString m_format;
};

#endif // STYLEDDATEDELEGATE_H
