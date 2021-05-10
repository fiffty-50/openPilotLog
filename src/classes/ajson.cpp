#include "ajson.h"

void AJson::exportDatabase()
{
    const QList<QPair<QLatin1String, ADatabaseTables>> tables {
        qMakePair(Opl::Db::TABLE_TAILS, ADatabaseTables::tails),
        qMakePair(Opl::Db::TABLE_PILOTS, ADatabaseTables::pilots),
        qMakePair(Opl::Db::TABLE_CURRENCIES, ADatabaseTables::currencies),
        qMakePair(Opl::Db::TABLE_FLIGHTS, ADatabaseTables::flights),
    };

    for (const auto &pair : tables){
        QJsonArray arr;
        const auto rows = aDB->getTable(pair.second);
        for (const auto & row : rows)
            arr.append(QJsonObject::fromVariantMap(row));
        QJsonDocument doc(arr);
        writeJson(doc, pair.first + QLatin1String(".json"));
    }
}

void AJson::writeJson(const QJsonDocument &doc, const QString &file_name)
{
    QFile pilots_out(AStandardPaths::asChildOfDir(AStandardPaths::JSON,file_name));
    pilots_out.open(QFile::WriteOnly);
    pilots_out.write(doc.toJson());
    pilots_out.close();
}

void AJson::readJson(const QString &file_name)
{
    TODO << "implement..." << file_name;
}
