#ifndef DBSUMMARY_H
#define DBSUMMARY_H

#include "src/database/database.h"

namespace OPL {

/*!
 * \brief Enumerates the QHash keys used when summarising a database
 */
enum class DbSummaryKey {
    total_flights,
    total_tails,
    total_pilots,
    last_flight,
    total_time,
};

/*!
 * \brief The DbSummary class collects summaries of entries and relevant data from the database.
 *
 * \details Summaries are used mainly in the BackupWidget to give the user a quick overview about the
 * contents of a database so they can decide which backup to delete/restore. This class is a helper class
 * to avoid interfacing with the database directly. It uses a separate database connection in order to not
 * interfere with the currently active database.
 */
class DbSummary : public QObject
{
public:
    DbSummary() = default;

    /*!
     * \brief Return a summary of a database
     * \details Creates a summary of the database giving a quick overview of the relevant contents. The
     * function runs several specialised SQL queries to create a QHash<DatabaseSummaryKey, QString> containing
     * Total Flight Time, Number of unique aircraft and pilots, as well as the date of last flight. Uses a temporary
     * database connection separate from the default connection in order to not tamper with the currently active
     * database connection. The full path to the database to be summarized has to be provided.
     */
    static const QMap<DbSummaryKey, QString> databaseSummary(const QString& db_path);

    /*!
     * \brief returns a short summary string of the database, containing total time and date of last flight.
     */
    static const QString summaryString(const QString& db_path);

private:
    Q_OBJECT
    inline const static QString SQLITE_DRIVER  = QStringLiteral("QSQLITE");

};


} // namespace OPL

#endif // DBSUMMARY_H
