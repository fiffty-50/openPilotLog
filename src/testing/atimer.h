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
#ifndef ATIMER_H
#define ATIMER_H

#include <QObject>
#include <chrono>
#include <QDebug>
#include "src/functions/alog.h"

/*!
 * \brief The ATimer class provides an easy to use performance timer.
 *
 * It automatically stops when going out of scope and prints the elapsed time
 * to the console. Intermediate timings can be manually obtained with timeNow().
 *
 * It can be given a QObject as a parent to time its lifetime or can be used without
 * parent in any context.
 */
class ATimer : public QObject
{
    Q_OBJECT
public:
    ATimer(QObject* parent = nullptr);
    ~ATimer();

    /*!
     * \brief timeNow takes an intermediate timing and returns miliseconds elapsed.
     */
    long timeNow();
private:

    std::chrono::high_resolution_clock::time_point start;

    std::chrono::high_resolution_clock::time_point intermediate_point;

    std::chrono::high_resolution_clock::time_point stop;

    double duration;

};


#endif // ATIMER_H
