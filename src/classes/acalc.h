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

#ifndef CALC_H
#define CALC_H

#include "src/database/db.h"
#include "src/classes/aircraft.h"
#include "src/classes/flight.h"
#include "src/classes/settings.h"
#include <QDateTime>
#include <cmath>
#include <QDebug>
/*!
 * \brief The calc class provides functionality for various calculations that are performed
 * outside of the database. This includes tasks like converting different units and formats,
 * or functions calculating block time or night time.
 */
namespace ACalc {

QTime blocktime(QTime tofb, QTime tonb);

QString minutesToString(QString blockminutes);

int stringToMinutes(QString time);

int QTimeToMinutes(QTime time);

double radToDeg(double rad);

double degToRad(double deg);

double radToNauticalMiles(double rad);

double greatCircleDistance(double lat1, double lon1, double lat2, double lon2);

double greatCircleDistanceBetweenAirports(QString dept, QString dest);

QVector<QVector<double>> intermediatePointsOnGreatCircle(double lat1,
                                                         double lon1,
                                                         double lat2,
                                                         double lon2,
                                                         int tblk);

double solarElevation(QDateTime utc_time_point, double lat, double lon);

int calculateNightTime(const QString &dept, const QString &dest, QDateTime departureTime, int tblk, int nightAngle);

bool isNight(QString icao, QDateTime event_time, int nightAngle);

QString formatTimeInput(QString user_input);

void updateAutoTimes(int acft_id);

void autoTimes(Flight, Aircraft);

void updateNightTimes();
} // namespace ACalc


#endif // CALC_H
