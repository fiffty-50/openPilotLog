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
#ifndef DBSETUP_H
#define DBSETUP_H

#include <QCoreApplication>
#include <QStringBuilder>
#include <QEventLoop>

#define DATABASE_REVISION 16

const auto TEMPLATE_URL = QStringLiteral("https://raw.githubusercontent.com/fiffty-50/openpilotlog/develop/assets/database/templates/");

/*!
 * \brief The ADataBaseSetup class is responsible for the inital setup of the database when
 * the application is first launched. It creates the database in the specified default
 * location and creates all required tables and views. It can also be used to reset the
 * database currently used
 */
class ADataBaseSetup
{
public:
    static void debug();

    static bool createDatabase();

    static bool downloadTemplates();

    static bool backupOldData();

    static bool fillTemplates();

    static bool importDefaultData(bool use_local_data);

    static bool resetToDefault();

    static bool commitData(QVector<QStringList> from_csv, const QString &table_name);

private:

    static bool createSchemata(const QStringList &statements);


};

#endif // DBSETUP_H
