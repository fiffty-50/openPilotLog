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
#ifndef DBSETTINGS_H
#define DBSETTINGS_H

#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

/*!
 * \brief The dbSettings class provides functionality for retreiving settings
 * from the database. In general, most values are provided as either QString or
 * QVector<QString>.
 */
class dbSettings
{
public:

    static void storeSetting(int setting_id, QString setting_value);

    static QString retreiveSetting(int setting_id);

    static QVector<QString> retreiveSettingInfo(QString setting_id);

    static QString sqliteversion();
};

#endif // DBSETTINGS_H
