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
#ifndef AJSON_H
#define AJSON_H
#include "src/opl.h"
#include "src/database/adatabase.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>

/*!
 * \brief The AJSON class is responsible for parsing the database (sqlite) to and from JSON.
 */
class AJson
{
public:
    AJson();
    AJson(QFileInfo database_file);

    /*!
     * \brief exportDatabase exports the currently active database to JSON.
     *
     * \details QJsonObject can be constructed from QHash<QString, QVariant> aka QVariantmap.
     * All rows are put into a QJsonArray and the Array is embedded into a QJsonDocument, which
     * can be exported. The files are created at AStandardPaths::JSON.
     */
    static void exportDatabase();

    /*!
     * \brief importDatabase imports a dataset from JSON. Files need to be at AStanardPaths::JSON for now.
     */
    static void importDatabase();

    /*!
     * \brief readToDocument reads data from a JSON file into a QJSonDocument
     * \param file_path
     */
    static QJsonDocument readFileToDoc(const QString &file_path);

    /*!
     * \brief writes a QJsonDocument to a file
     */
    static void writeDocToFile(const QJsonDocument &doc, const QString& file_name);

private:

    /*!
     * \brief holds the tables with userdata that need to be processed.
     */
    const static QList<QPair<TableName_T, ADatabaseTable>> tables;

};

#endif // AJSON_H
