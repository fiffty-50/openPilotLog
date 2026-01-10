/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
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
 *
 */
#include "databasesetup.h"

bool DatabaseSetup::createTables()
{
    for (const auto &table_file : m_tables) {
        if (!executeSqlFile(table_file)) {
            LOG << "Error creating database tables.";
            return false;
        }
    }
    LOG << "Database tables created successfully.";
    return true;
}

bool DatabaseSetup::createViews()
{
    for (const auto &view_file : m_views) {
        if (!executeSqlFile(view_file)) {
            LOG << "Error creating database views.";
            return false;
        }
    }
    LOG << "View creation unimplemented";
    //LOG << "Database views created successfully.";
    return true;
}

bool DatabaseSetup::executeSqlFile(const QString& file_path)
{
    QFile f(file_path);
    if(!f.open(QIODevice::ReadOnly)) {
        LOG << "Unable to read database sql file: " << file_path << " - " << f.errorString();
        return false;
    }
    const QByteArray filedata = f.readAll();

    // create individual queries for each table/view
    auto list = filedata.split(';');
    // but make sure no empty lines are parsed
    list.removeAll(QByteArray("\n"));
    list.removeAll(QByteArray("\r\n"));

    // Create and execute Queries
    for (const auto &query_string : std::as_const(list)) {
        QSqlQuery q;
        q.prepare(query_string);
        if (!q.exec()) {
            LOG << "Unable to execute query from file: " << file_path;
            LOG << q.lastQuery();
            LOG << q.lastError();
            return false;
        }
    }
    LOG << "SQL file executed successfully: " << file_path;
    return true;
}

bool DatabaseSetup::importTemplateData()
{
    if (m_useOnlineTemplateData) {
        return importOnlineTemplateData();
    } else {
        return importLocalTemplateData();
    }
}

bool DatabaseSetup::importLocalTemplateData()
{
    for( auto it = m_templateData.cbegin(); it != m_templateData.cend(); ++it ) {
        const OPL::DbTable table = it.key();
        const QString table_name = OPL::GLOBALS->getDbTableName(table);
        const QString file_path = it.value();
        LOG << "Importing local template data from: " << file_path << " into table: " << table_name;

        // clear table to make sure it is empty before import
        QSqlQuery q;
        q.prepare(QLatin1String("DELETE FROM ") + OPL::GLOBALS->getDbTableName(table));
        if (!q.exec()) {
            LOG << "Error clearing template data table: " << table_name << " - " << q.lastError().text();
            return false;
        }

        // Prepare import data
        const QJsonArray dataToCommit = JsonHelper::readFileToDoc(file_path).array();

        // Commit data
        if(!DB->commit(dataToCommit, table)) {
            LOG << "Error importing template data into table: " << table_name << " - " << DB->lastError.text();
            return false;
        }
    }
    LOG << "Local template data imported successfully.";
    return true;
}

bool DatabaseSetup::importOnlineTemplateData()
{
    Q_UNIMPLEMENTED();
    LOG << "Online template data import unimplemented.";
    return false;
}
