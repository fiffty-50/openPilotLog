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
#ifndef AIRCRAFT_H
#define AIRCRAFT_H
#include <QCoreApplication>
#include <QVector>
#include "src/database/db.h"

/*!
 * \brief The aircraft class
 *
 */
class aircraft
{
public:
    enum database {tail, acft};

    aircraft();
    aircraft(int database_id, database table);
    aircraft(QString registration);

    QString id;
    QString registration;
    QString company;
    QString make;
    QString model;
    QString variant;
    bool singlepilot    = false;
    bool multipilot     = false;
    bool singleengine   = false;
    bool multiengine    = false;
    bool unpowered      = false;
    bool piston         = false;
    bool turboprop      = false;
    bool jet            = false;
    bool light          = false;
    bool medium         = false;
    bool heavy          = false;
    bool super          = false;

    // Functions
    static QVector<QString> toVector(aircraft);

    // Debug functionality
    void print();
    QString debug();
    operator QString() { return debug(); } //overload for compatibility with qDebug()
};

#endif // AIRCRAFT_H
