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
#include "ajson.h"
#include "src/database/adatabase.h"

const QList<QPair<TableName_T, ADatabaseTable>> AJson::tables {
    qMakePair(OPL::Db::TABLE_TAILS, ADatabaseTable::tails),
    qMakePair(OPL::Db::TABLE_PILOTS, ADatabaseTable::pilots),
    qMakePair(OPL::Db::TABLE_CURRENCIES, ADatabaseTable::currencies),
    qMakePair(OPL::Db::TABLE_FLIGHTS, ADatabaseTable::flights),
};

void AJson::exportDatabase()
{
    for (const auto &pair : tables){
        QJsonArray array;
        const auto rows = aDB->getTable(pair.second);

        for (const auto &row : rows)
            array.append(QJsonObject::fromVariantHash(row));

        QJsonDocument doc(array);
        writeDocToFile(doc, pair.first + QLatin1String(".json"));
    }
}

void AJson::importDatabase()
{
    TODO << "Another function should do some checking here if data exists etc...";
    TODO << "Make the function take a list of files/fileinfo as arguments";
    // clear tables
    QSqlQuery q;
    // make sure flights is cleared first due to foreign key contstraints
    q.prepare(QStringLiteral("DELETE FROM FLIGHTS"));
    q.exec();
    for (const auto & pair : tables) {
        q.prepare(QLatin1String("DELETE FROM ") + pair.first);
        q.exec();
        const auto doc = readFileToDoc(AStandardPaths::asChildOfDir(AStandardPaths::JSON,
                                                               pair.first + QLatin1String(".json")));
        aDB->commit(doc.array(), pair.first);
    }
}

QJsonDocument AJson::readFileToDoc(const QString &file_path)
{
    QFile file(file_path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString raw = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(raw.toUtf8());
    return doc;
}

void AJson::writeDocToFile(const QJsonDocument &doc, const QString &file_name)
{
    QFile out(AStandardPaths::asChildOfDir(AStandardPaths::JSON,file_name));
    out.open(QFile::WriteOnly);
    out.write(doc.toJson());
    out.close();
}
