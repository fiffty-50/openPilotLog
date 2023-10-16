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
 * The tails table holds the various tails the user has added to his logbook.
 * Within the program the term aircraft refers to an aircraft type and is stored
 * in the aircraft database and is used as a template. A tail is a specific instance
 * of an aircraft which is identified by its registration (tail).
 */
class TailEntry : public Row
{
    const static inline QString TABLE_NAME = QStringLiteral("tails");
public:
    TailEntry();
    TailEntry(const RowData_T &row_data);
    TailEntry(int row_id, const RowData_T &row_data);
    const QString getTableName() const override;

    /*!
     * \brief Return the aircrafts registration
     * \return
     */
    const QString registration() const;
    /*!
     * \brief Return the aircraft type
     */
    const QString type()         const; //TODO - Create String for make-model-variant

    /*!
     * \brief The entries row id in the database
     */
    const static inline QString ROWID            = QStringLiteral("tail_id");
    /*!
     * \brief The aircrafts registration ("LN-ENL", "D-ABCD")
     */
    const static inline QString REGISTRATION     = QStringLiteral("registration");
    /*!
     * \brief The company the aircraft is operated by
     */
    const static inline QString COMPANY          = QStringLiteral("company");
    /*!
     * \brief The aircrafts manufacturer
     */
    const static inline QString MAKE             = QStringLiteral("make");
    /*!
     * \brief The aircraft model
     */
    const static inline QString MODEL            = QStringLiteral("model");
    /*!
     * \brief The aircraft model variant
     */
    const static inline QString VARIANT          = QStringLiteral("variant");
    /*!
     * \brief Wether the aircraft requires more than one pilot (stored in the database as boolean)
     */
    static const inline QString MULTI_PILOT = QStringLiteral("multipilot");
    /*!
     * \brief Wether the aircraft has more than one engine (stored in the database as a boolean)
     */
    static const inline QString MULTI_ENGINE = QStringLiteral("multiengine");
    /*!
     * \brief The aircrafts engine type. Stored in the database as an integer
     * \details
     * <ul>
     * <li> 0 - Single Engine Piston <\li>
     * <li> 1 - Multi Engine Piston <\li>
     * <li> 2 - Turboprop <\li>
     * <li> 3 - Jet <\li>
     * <\ul>
     */
    static const inline QString ENGINE_TYPE = QStringLiteral("engineType");
    /*!
     * \brief The aircrafts weight class. Stored in the database as an integer
     * \details
     * <ul>
     * <li> 0 - Light <\li>
     * <li> 1 - Medium <\li>
     * <li> 2 - Heavy <\li>
     * <li> 3 - Super Heavy <\li>
     * <\ul>
     */
    static const inline QString WEIGHT_CLASS = QStringLiteral("weightClass");
};

} // namespace OPL

#endif // TAILENTRY_H
