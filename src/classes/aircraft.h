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

class aircraft
{
public:
    aircraft();

    QString id;
    QString registration;
    QString company;
    QString make;
    QString model;
    QString variant;
    bool singlepilot;
    bool multipilot;
    bool singleengine;
    bool multiengine;
    bool unpowered;
    bool piston;
    bool turboprop;
    bool jet;
    bool light;
    bool medium;
    bool heavy;

    // Functions
    static aircraft         fromTails(int tail_id);
    static aircraft         fromAircraft(int acft_id);
    static QVector<QString> toVector(aircraft);

    // Debug functionality
    void print();
    QString debug();
    operator QString() { return debug(); }
};

#endif // AIRCRAFT_H
