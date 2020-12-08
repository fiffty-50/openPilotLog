#include "Db.h"

using namespace experimental;

bool DB::init()
{
    const QString driver("QSQLITE");

    if (!QSqlDatabase::isDriverAvailable(driver)) return false;

    QDir directory("data");
    QString databaseLocation = directory.filePath("logbook.db");
    QSqlDatabase db = QSqlDatabase::addDatabase(driver);
    db.setDatabaseName(databaseLocation);

    if (!db.open()) return false;

    DEB("Database connection established.");
    // Enable foreign key restrictions
    QSqlQuery query("PRAGMA foreign_keys = ON;");
    // Retreive database layout and store in member variables
    tableNames = db.tables();

    QStringList columnNames;
    for (const auto &table : tableNames) {
        columnNames.clear();
        QSqlRecord fields = db.record(table);
        for (int i = 0; i < fields.count(); i++) {
            columnNames.append(fields.field(i).name());
            tableColumns.insert(table, columnNames);
        }
    }
    return true;
}
