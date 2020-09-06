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
#include "dbsettings.h"


/*
 * Settings Database Related Functions
 */
/*!
 * \brief storesetting Updates a stored setting in the database
 * \param setting_id
 * \param setting_value
 */
void dbSettings::storeSetting(int setting_id, QString setting_value)
{
    QSqlQuery query;
    query.prepare("UPDATE settings "
                  "SET  setting = ? "
                  "WHERE setting_id = ?");
    query.addBindValue(setting_value);
    query.addBindValue(setting_id);
    query.exec();
}

/*!
 * \brief retreiveSetting Looks up a setting in the database and returns its value
 * \param setting_id
 * \return setting value
 */
QString dbSettings::retreiveSetting(int setting_id)
{
    QSqlQuery query;
    query.prepare("SELECT setting FROM settings WHERE setting_id = ?");
    query.addBindValue(setting_id);
    query.exec();

    QString setting = "-1";

    while(query.next()){
        setting = query.value(0).toString();
    }
    return setting;
}

/*!
 * \brief retreiveSettingInfo Looks up a setting in the database and returns its value and description
 * \param setting_id
 * \return {setting_id, setting, description}
 */
QVector<QString> dbSettings::retreiveSettingInfo(QString setting_id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM settings WHERE setting_id = ?");
    query.addBindValue(setting_id);
    query.exec();

    QVector<QString> setting;

    while(query.next()){
        setting.append(query.value(0).toString());
        setting.append(query.value(1).toString());
        setting.append(query.value(2).toString());
    }
    return setting;
}

QString dbSettings::sqliteversion()
{
    QSqlQuery version;
    version.prepare("SELECT sqlite_version()");
    version.exec();
    QString result;

    while (version.next()) {
        result.append(version.value(0).toString());
    }
    return result;
}
