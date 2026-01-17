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
 * of an aircraft type, and is identified by its alphanumeric registration (tail number).
 */
class TailEntry : public Row
{
public:
    TailEntry();
    TailEntry(const RowData_T &row_data);
    TailEntry(int row_id, const RowData_T &row_data);

    /*!
     * \brief A TailEntry is valid if it contains at least a registration and a valid type id.
     */
    bool isValid() const override;

    // getters and setters
    

    /*!
     * \brief Get the aircrafts registration (tail number)
     */
    QString getRegistration() const { return m_rowData.value(REGISTRATION).toString(); }
    
    /*!
     * \brief Get the company the aircraft is operated by
     */
    QString getCompany() const { return m_rowData.value(COMPANY).toString(); }
    
    /*!
     * \brief Get any remarks associated with the tail entry
     */
    QString getRemarks() const { return m_rowData.value(REMARKS).toString(); }
    
    /*!
     * \brief Get the aircrafts in-service date. This value must not be missing or invalid.
     */
    QDate getInServiceDate() const { return QDate::fromJulianDay(m_rowData.value(IN_SERVICE_DATE).toInt()); }
    
    /*!
     * \brief Get the aircrafts out-of-service date. This value may be invalid (null) for indefinite validity.
     */
    QDate getOutOfServiceDate() const { return QDate::fromJulianDay(m_rowData.value(OUT_OF_SERVICE_DATE).toInt()); }

    /*!
     * \brief Get the associated aircraft type id (foreign key to aircraft_types)
     */
    int getTypeId() const { return m_rowData.value(TYPE_ID).toInt(); }

    /*!
     * \brief Set the aircrafts registration (tail number)
     */
    bool setRegistration(const QString &registration);

    /*!
     * \brief Set the company the aircraft is operated by
     */
    void setCompany(const QString &company) { m_rowData.insert(COMPANY, company); }

    /*!
     * \brief Set any remarks associated with the tail entry
     */
    void setRemarks(const QString &remarks) { m_rowData.insert(REMARKS, remarks); }

    /*!
     * \brief Set the aircrafts in-service date. This date must not be missing or invalid.
     */
    bool setInServiceDate(const QDate &date);
    
    /*!
     * \brief Set the aircrafts out-of-service date. This date may be invalid (null) for indefinite validity.
     */
    bool setOutOfServiceDate(const QDate &date);

    /*!
     * \brief Set the associated aircraft type id (foreign key to aircraft_types)
     * \details The type id must refer to a valid aircraft type in the database.
     * if not, the function returns false and does not set the value.
     */
    bool setTypeId(int typeId);

private:
    const static inline QString TABLE_NAME       = QStringLiteral("aircraft_tails");
    const static inline QString ROWID            = QStringLiteral("tail_id");
    const static inline QString TYPE_ID 		 = QStringLiteral("aircraft_type_id");
    const static inline QString REGISTRATION     = QStringLiteral("registration");
    const static inline QString COMPANY          = QStringLiteral("company");
    const static inline QString REMARKS			 = QStringLiteral("remarks");
    const static inline QString IN_SERVICE_DATE	 = QStringLiteral("in_service_jd");
    const static inline QString OUT_OF_SERVICE_DATE = QStringLiteral("out_of_service_jd");

    const static inline QStringList FIELDS = {
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
