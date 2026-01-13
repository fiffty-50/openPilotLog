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
#ifndef TAILENTRY_H
#define TAILENTRY_H
#include "src/database/row.h"

namespace OPL {

/*!
 * \brief A Row representing a Tail (Registration) entry.
 * \details
 * The aircraft_tails table holds the various tails the user has added to his logbook.
 * Within the program the term aircraft refers to an aircraft type and is stored
 * in the aircraft_types database. A tail is a specific instance
 * of an aircraft which is identified by its alphanumeric registration (tail number).
 */
class TailEntry : public Row
{
    const static inline QString TABLE_NAME = QStringLiteral("aircraft_tails");
public:
    TailEntry();
    TailEntry(const RowData_T &row_data);
    TailEntry(int row_id, const RowData_T &row_data);
    const QString getTableName() const override;

    /*!
     * \brief Check if the data contained in this entry complies with the database constraints
     */
    bool isValid() const override;

    /*!
     * \brief initialise the rowData map with NULL for all values
     */
    void clear();

    // getters and setters
    QString getRegistration() const { return rowData.value(REGISTRATION).toString(); }
    QString getCompany() const { return rowData.value(COMPANY).toString(); }
    QString getRemarks() const { return rowData.value(REMARKS).toString(); }
    QDate getInServiceDate() const { return QDate::fromJulianDay(rowData.value(IN_SERVICE_DATE).toInt()); }
    QDate getOutOfServiceDate() const { return QDate::fromJulianDay(rowData.value(OUT_OF_SERVICE_DATE).toInt()); }
    int getTypeId() const { return rowData.value(TYPE_ID).toInt(); }

    bool setRegistration(const QString &registration);
    void setCompany(const QString &company) { rowData.insert(COMPANY, company); }
    void setRemarks(const QString &remarks) { rowData.insert(REMARKS, remarks); }
    bool setInServiceDate(const QDate &date);
    bool setOutOfServiceDate(const QDate &date);
    bool setTypeId(int typeId);

private:
    /*!
     * \brief The entries row id in the database
     */
    const static inline QString ROWID            = QStringLiteral("tail_id");

    /*!
     * \brief The entries associated aircraft type (Foreign Key to aircraft_types)
     */
    const static inline QString TYPE_ID 		 = QStringLiteral("aircraft_type_id");
    /*!
     * \brief The aircrafts registration ("LN-ENL", "D-ABCD")
     */
    const static inline QString REGISTRATION     = QStringLiteral("registration");
    /*!
     * \brief The company the aircraft is operated by
     */
    const static inline QString COMPANY          = QStringLiteral("company");

    const static inline QString REMARKS			 = QStringLiteral("remarks");
    /*!
     * \brief IN_SERVICE_DATE The start date of this aircraft registration
     */
    const static inline QString IN_SERVICE_DATE	 = QStringLiteral("in_service_jd");

    /*!
     * \brief OUT_OF_SERVICE_DATE The end date of this aircraft registration
     */
    const static inline QString OUT_OF_SERVICE_DATE = QStringLiteral("out_of_service_jd");

    const static inline QStringList fields = {
        TYPE_ID,
        REGISTRATION,
        COMPANY,
        REMARKS,
        IN_SERVICE_DATE,
        OUT_OF_SERVICE_DATE
    };

};

} // namespace OPL

#endif // TAILENTRY_H
