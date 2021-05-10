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
     * \brief exportDatabase exports the currently active database to JSON. Files are created at AStandardPaths::JSON.
     */
    static void exportDatabase();

private:

    /*!
     * \brief writes a QJsonDocument to a file
     */
    static void writeJson(const QJsonDocument &doc, const QString& file_name);

    /*!
     * \brief To Do - Reads a file into a QJasonDocument
     */
    static void readJson(const QString& file_name);
};

#endif // AJSON_H
