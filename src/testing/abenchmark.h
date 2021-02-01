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
#ifndef ABENCHMARK_H
#define ABENCHMARK_H

#include <QObject>
#include "src/testing/adebug.h"

/*!
 * \brief The ABenchmark class provides quick access to benchmarking two functions for
 * the purpose of performance testing.
 *
 */
class ABenchmark
{
public:
    ABenchmark();

    ABenchmark(void (*function_one)(), void (*function_two)(), int number_of_runs);

    ABenchmark(bool (*function_one)(), bool (*function_two)(), int number_of_runs);

};

#endif // ABENCHMARK_H
