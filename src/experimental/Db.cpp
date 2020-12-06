#include "Db.h"

using namespace experimental;

bool DB::init()
{
    const QString driver("QSQLITE");

    if (QSqlDatabase::isDriverAvailable(driver)) {

        QDir directory("data");
        QString databaseLocation = directory.filePath("logbook.db");
        QSqlDatabase db = QSqlDatabase::addDatabase(driver);
        db.setDatabaseName(databaseLocation);

        if (!db.open()) {
            DEB("DatabaseConnect - ERROR: " << db.lastError().text());
            return false;
        } else {
            DEB("Database connection established.");
            // Enable foreign key restrictions
            QSqlQuery query("PRAGMA foreign_keys = ON;");
            // Retreive database layout and store in member variables
            tables = db.tables();

            QStringList columnNames;
            for (const auto &table : tables) {
                columnNames.clear();
                QSqlRecord fields = db.record(table);
                for (int i = 0; i < fields.count(); i++) {
                    columnNames.append(fields.field(i).name());
                    database_layout.insert(table, columnNames);
                    ///[F] database_layout.value("pilots") gives you the columns for the pilots table
                }
            }
            return true;
        }
    } else {
        DEB("DatabaseConnect - ERROR: no driver " << driver << " available");
        return false;
    }
}
