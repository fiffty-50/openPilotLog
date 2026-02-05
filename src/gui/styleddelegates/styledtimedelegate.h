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
#ifndef STYLEDTIMEDELEGATE_H
#define STYLEDTIMEDELEGATE_H

#include <QStyledItemDelegate>

/*!
 * \brief The StyledTimeDelegate class is used to convert the database time format to a
 * human-readable format.
 * \details The database stores time values as an integer representing minutes elapsed since
 * midnight. This delegate can be used in a QTableView to format the database value as "hh:mm"
 */
class StyledTimeDelegate : public QStyledItemDelegate {
  public:
    explicit StyledTimeDelegate(const QString &format, QObject *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;

  private:
    QString m_format;
};

#endif // STYLEDTIMEDELEGATE_H
