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
#ifndef DBSETUP_H
#define DBSETUP_H

#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

/*!
 * \brief The dbSetup class is responsible for the inital setup of the database when
 * the application is first launched. It creates the database in the specified default
 * location and creates all required tables and views.
 */
class dbSetup
{
public:
    static void connect();

    static void createTables();

    static void createViews();
};

#endif // DBSETUP_H
