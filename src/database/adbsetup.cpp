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
#include "adbsetup.h"
#include "src/opl.h"
#include "src/database/adatabase.h"
#include "src/functions/alog.h"
#include "src/classes/ajson.h"

namespace aDbSetup {

// const auto TEMPLATE_URL = QStringLiteral("https://raw.githubusercontent.com/fiffty-50/openpilotlog/tree/main/assets/database/templates/");
const static auto TEMPLATE_URL = QStringLiteral("https://raw.githubusercontent.com/fiffty-50/openpilotlog/develop/assets/database/templates/");

const static QStringList USER_TABLES = {
    QStringLiteral("flights"),
    QStringLiteral("pilots"),
    QStringLiteral("tails")
};
const static QStringList TEMPLATE_TABLES= {
    QStringLiteral("aircraft"),
    QStringLiteral("airports"),
    QStringLiteral("currencies"),
    QStringLiteral("changelog")
};

bool createDatabase()
{
    // Read Database layout from sql file
    QFile f(OPL::Assets::DATABASE_SCHEMA);
    f.open(QIODevice::ReadOnly);
    QByteArray filedata = f.readAll();
    // create individual queries for each table/view
    auto list = filedata.split(';');

    // Commit
    QSqlQuery q;
    QVector<QSqlError> errors;
    // Create Tables
    for (const auto &query_string : list) {
        q.prepare(query_string);
        if (!q.exec()) {
            errors.append(q.lastError());
            LOG << "Unable to execute query: ";
            LOG << q.lastQuery();
            LOG << q.lastError();
        }
    }

    aDB->updateLayout();

    if (errors.isEmpty()) {
        LOG << "Database succesfully created.";
        return true;
    } else {
        LOG << "Database creation has failed. The following error(s) have ocurred: ";
        for (const auto &error : qAsConst(errors)) {
            LOG << error.type() << error.text();
        }
        return false;
    }
}

bool commitData(const QJsonArray &json_arr, const QString &table_name)
{
    //aDB->updateLayout(); // needed?
    QSqlQuery q;

    // create insert statement
    QString statement = QLatin1String("INSERT INTO ") + table_name + QLatin1String(" (");
    QString placeholder = QStringLiteral(") VALUES (");
    for (const auto &column_name : aDB->getTableColumns(table_name)) {
        statement += column_name + ',';
        placeholder.append(QLatin1Char(':') + column_name + QLatin1Char(','));
    }

    statement.chop(1);
    placeholder.chop(1);
    placeholder.append(')');
    statement.append(placeholder);

    q.prepare(QStringLiteral("BEGIN EXCLUSIVE TRANSACTION"));
    q.exec();
    //DEB << statement;
    for (const auto &entry : json_arr) {
        q.prepare(statement);

        auto object = entry.toObject();
        const auto keys = object.keys();
        for (const auto &key : keys){
            object.value(key).isNull() ? q.bindValue(key, QVariant(QVariant::String)) :
                                         q.bindValue(QLatin1Char(':') + key, object.value(key).toVariant());
        }

        q.exec();
    }

    q.prepare(QStringLiteral("COMMIT"));
    if (q.exec())
        return true;
    else
        return false;
}

bool importTemplateData(bool use_local_ressources)
{
    //QSqlQuery q;
    // reset template tables
    const auto table_names = aDB->getTemplateTableNames();
    for (const auto& table_name : table_names) {

        //clear table
        //q.prepare(QLatin1String("DELETE FROM ") + table_name);
        //if (!q.exec()) {
        //    DEB << "Error: " << q.lastError().text();
        //    return false;
        //}

        //Prepare data
        QJsonArray data_to_commit;
        QString error_message("Error importing data ");

        if (use_local_ressources) {
            data_to_commit = AJson::readFileToDoc(QLatin1String(":database/templates/")
                                      + table_name + QLatin1String(".json")).array();
            error_message.append(QLatin1String(" (ressource) "));
        } else {
            data_to_commit = AJson::readFileToDoc(AStandardPaths::directory(
                                          AStandardPaths::Templates).absoluteFilePath(
                                          table_name + QLatin1String(".json"))).array();
            error_message.append(QLatin1String(" (downloaded) "));
        }

        // commit Data from Array
        if (!commitData(data_to_commit, table_name)) {
            LOG << error_message;
            return false;
        }
    } // for table_name
    return true;
}

bool resetUserData()
{
    QSqlQuery query;

    // clear user tables
    for (const auto& table : USER_TABLES) {
        query.prepare("DELETE FROM " + table);
        if (!query.exec()) {
            DEB << "Error: " << query.lastError().text();
        }
    }
    return true;
}

} // namespace aDbSetup
