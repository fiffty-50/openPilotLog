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
#ifndef AIRCRAFTENTRY_H
#define AIRCRAFTENTRY_H
#include "src/database/row.h"

namespace OPL {

/*!
 * \brief A Row representing an Aircraft entry.
 * \details
 * In this context an aircraft refers to the aircraft TYPE (Boeing 737, Airbus A320,...) and
 * could be seen as an analogy for a class. The aircraft 'instances' are called TAILS and are
 * stored in the tails database (A Boeing 737 with registration (tail) LN-ENL for example).
 *
 * Aircraft = All aircraft with a common type that share its traits
 * Tail - A specific aircraft of that type
 *
 * The aircraft table in the database contains templates of various aircraft types
 * and is used to provide auto-completion when the user adds a new tail to the logbook.
 */
class AircraftEntry : public Row
{
    const static inline QString TABLE_NAME = QStringLiteral("aircraft");
public:
    AircraftEntry();
    AircraftEntry(const RowData_T &row_data);
    AircraftEntry(int row_id, const RowData_T &row_data);
    const QString getTableName() const override;

    /*!
     * \brief The aircrafts manufacturer (Airbus, Boeing,...)
     */
    static const inline QString MAKE = QStringLiteral("make");
    /*!
     * \brief The aircraft model (A320, B737,...)
     */
    static const inline QString MODEL = QStringLiteral("model");
    /*!
     * \brief The aircraft variant( A320-<b>200<\b>, B737-<b>800<\b>)
     */
    static const inline QString VARIANT = QStringLiteral("variant");
    /*!
     * \brief (optional) The aircraft common name ("Beluga",...)
     */
    static const inline QString NAME = QStringLiteral("name");
    /*!
     * \brief The aircraft types iata code
     */
    static const inline QString IATA = QStringLiteral("iata");
    /*!
     * \brief The aircraft types icao code
     */
    static const inline QString ICAO = QStringLiteral("icao");
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


#endif // AIRCRAFTENTRY_H
