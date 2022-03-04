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
#ifndef ADBSETUP_H
#define ADBSETUP_H

#include <QCoreApplication>

/*!
 * \brief The aDbSetup namespace is responsible for the inital setup of the database when
 * the application is first launched. It creates the database in the specified default
 * location and creates all required tables and views.
 */
namespace aDbSetup
{

/*!
 * \brief createDatabase runs a number of CREATE queries that create the database tables and columns.
 * \return
 */
bool createDatabase();

/*!
 * \brief commitData commits the data read from a JSON file into a table in the database.
 */
bool commitData(const QJsonArray &json_arr, const QString &table_name);

/*!
 * \brief importTemplateData fills an empty database with the template
 * data (Aircraft, Airports, currencies, changelog) as read from the JSON
 * templates.
 * \param use_local_ressources determines whether the included ressource files
 * or a previously downloaded file should be used.
 * \return
 */
bool importTemplateData(bool use_local_ressources);

/*!
 * \brief  Empties all user-generated content in the database.
 * \return true on success
 */
bool resetUserData();
}; // namespace aDbSetup

#endif // ADBSETUP_H
