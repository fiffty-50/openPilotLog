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
#ifndef DBAPI_H
#define DBAPI_H

/* This header can be used to include database functionality for widgets
 * or classes that need it.
 */

// Qt modules
#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlDriver>
// openPilotLog classes
#include "dbsetup.h"
#include "dbsettings.h"
#include "dbairport.h"
#include "dbaircraft.h"
#include "dbflight.h"
#include "dbpilots.h"
#include "dbstat.h"
#include "dbvalidate.h"

#endif // DBAPI_H
