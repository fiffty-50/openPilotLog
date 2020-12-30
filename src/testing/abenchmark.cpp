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
#include "abenchmark.h"

ABenchmark::ABenchmark()
{

}

ABenchmark::ABenchmark(void (*function_one)(), void (*function_two)(), int number_of_runs)
{
        auto start1 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < number_of_runs; i++) {
            function_one();
        }
        auto stop1 = std::chrono::high_resolution_clock::now();
        DEB << "First Function execution finished (" << number_of_runs << " runs)";


        auto start2 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < number_of_runs; i++) {
            function_two();
        }
        auto stop2 = std::chrono::high_resolution_clock::now();
        DEB << "Second Function execution finished (" << number_of_runs << " runs)";

        DEB << "Execution time for function 1: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1).count()
            << "milliseconds.";
        DEB << "Execution time for function 2: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2).count()
            << "milliseconds.";
}

ABenchmark::ABenchmark(bool (*function_one)(), bool (*function_two)(), int number_of_runs)
{
    auto start1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < number_of_runs; i++) {
        function_one();
    }
    auto stop1 = std::chrono::high_resolution_clock::now();
    DEB << "First Function execution finished (" << number_of_runs << " runs)";


    auto start2 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < number_of_runs; i++) {
        function_two();
    }
    auto stop2 = std::chrono::high_resolution_clock::now();
    DEB << "Second Function execution finished (" << number_of_runs << " runs)";

    DEB << "Execution time for function 1: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(stop1 - start1).count()
        << "milliseconds.";
    DEB << "Execution time for function 2: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - start2).count()
        << "milliseconds.";
}
