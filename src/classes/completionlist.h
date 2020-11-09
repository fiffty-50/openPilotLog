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
#ifndef COMPLETIONLIST_H
#define COMPLETIONLIST_H

#include <QCoreApplication>
#include "src/database/db.h"

class CompleterTarget
{
public:
    enum targets {airports, pilots, registrations, aircraft, companies};
};


/*!
 * \brief The completionList class provides QStringLists to be used by a QCompleter
 */
class CompletionList
{
public:
    QStringList list;

    CompletionList();

    CompletionList(CompleterTarget::targets);
};

#endif // COMPLETIONLIST_H
