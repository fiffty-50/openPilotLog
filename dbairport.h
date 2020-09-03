/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
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
#ifndef DBAIRPORT_H
#define DBAIRPORT_H

#include <QCoreApplication>

/*!
 * \brief The dbAirport class provides functionality for retreiving airport related
 * data from the database.
 */
class dbAirport
{
public:

    static QString retreiveAirportNameFromIcaoOrIata(QString identifier);

    static QString retreiveAirportIdFromIcao(QString identifier);

    static bool checkICAOValid(QString identifier);

    static QVector<double> retreiveIcaoCoordinates(QString icao);

    static QStringList completeIcaoOrIata(QString icaoStub);

    static QStringList retreiveIataIcaoList();
};

#endif // DBAIRPORT_H
