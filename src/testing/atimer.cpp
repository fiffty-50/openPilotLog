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
#include "atimer.h"


ATimer::ATimer(QObject *parent) : QObject(parent)
{
     start = std::chrono::high_resolution_clock::now();
     if(parent == nullptr) {
         DEB("Starting Timer... ");
     } else {
         DEB("Starting Timer for: " << parent->objectName());
     }

}

ATimer::~ATimer()
{
    stop = std::chrono::high_resolution_clock::now();
    if(parent() == nullptr) {
        DEB("Execution time: "
                 << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()
                 << "milliseconds.");
    } else {
        DEB("Execution time for: " << parent()->objectName() << ": "
                 << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count()
                 << "milliseconds.");
    }
}

long ATimer::timeNow()
{
    intermediate_point = std::chrono::high_resolution_clock::now();
    if(parent() == nullptr) {
        DEB("Intermediate time: "
                 << std::chrono::duration_cast<std::chrono::milliseconds>(intermediate_point - start).count()
                 << "milliseconds.");
    } else {
        DEB("Intermediate time for: " << parent()->objectName() << ": "
                 << std::chrono::duration_cast<std::chrono::milliseconds>(intermediate_point - start).count()
                 << "milliseconds.");
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(intermediate_point - start).count();
}
