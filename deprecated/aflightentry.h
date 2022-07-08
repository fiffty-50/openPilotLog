/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#ifndef AFLIGHTENTRY_H
#define AFLIGHTENTRY_H

#include "src/classes/aentry.h"

class AFlightEntry : public AEntry {
public:
    AFlightEntry();
    AFlightEntry(RowId_T row_id);
    AFlightEntry(RowData_T table_data);

    /*!
     * \brief Returs a summary of the flight data, if struct holds data
     * \return "doft, dept, tofb, dest, tonb"
     */
    const QString summary();

};

#endif // AFLIGHTENTRY_H
