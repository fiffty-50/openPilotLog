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
#ifndef CURRENCYENTRY_H
#define CURRENCYENTRY_H
#include "src/database/row.h"

namespace OPL {

/*!
 * \brief A Row representing a Currency entry. See Row class for details.
 * \details An Entry in the currencies table represents an item with an expiry date
 * and is used to track those. Typical example would be medical licenses, type rating
 * validations or route certifications.
 */
class CurrencyEntry : public Row
{
public:

    enum Currency {Licence = 1, TypeRating = 2, LineCheck = 3, Medical = 4, Custom1 = 5, Custom2 = 6, TakeOffLanding = 7};

    CurrencyEntry() = delete;
    CurrencyEntry(const RowData_T &row_data) = delete;
    CurrencyEntry(int row_id, const RowData_T &row_data);

    const QString getTableName() const override;

    void setDisplayName(const QString& displayName);
    const QString getDisplayName() const;

    void setExpiryDate(const QDate &date);
    const QDate getExpiryDate() const;



private:
    /*!
    * \brief The sql column name for the expiry date
    */
    const static inline QString EXPIRYDATE  = QStringLiteral("expiryDate");

    /*!
     * \brief The sql column name for the row id
     */
    const static inline QString ROW_ID = QStringLiteral("currency_id");

    /*!
     * \brief The sql column name for the display name
     */
    const static inline QString DISPLAY_NAME = QStringLiteral("displayName");

    const static inline QString TABLE_NAME = QStringLiteral("currencies");
};

} // namespace OPL

#endif // CURRENCYENTRY_H
