#ifndef __DB_H__
#define __DB_H__

#include <QPair>
#include <QMap>
#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include "src/database/dbinfo.h"
#include "debug.h"

#include "aentry.h"

namespace experimental {

/*!
 * \brief The DB class encapsulates the SQL database by providing fast access
 * to hot database data.
 */
class ADataBase : public QObject {
    Q_OBJECT
private:
    TableNames tableNames;
    TableColumns tableColumns;
    static ADataBase* instance;
    ADataBase() = default;
public:
    // Ensure DB is not copiable or assignable
    ADataBase(const ADataBase&) = delete;
    void operator=(const ADataBase&) = delete;
    static ADataBase* getInstance();

    /*!
     * \brief The CompleterTarget enum provides the items for which QCompleter
     * completion lists are provided from the database.
     */
    enum CompleterTarget {airports, pilots, registrations, aircraft, companies};

    /*!
     * \brief Connect to the database and populate database information.
     */
    bool connect();

    /*!
     * \brief closes the database connection.
     */
    void disconnect();

    /*!
     * \brief Can be used to access the database connection.
     * \return The QSqlDatabase object pertaining to the connection.
     */
    static QSqlDatabase database();

    /*!
     * \brief Can be used to send a complex query to the database.
     * \param query - the full sql query statement
     * \param returnValues - the number of return values
     */
    QVector<QString> customQuery(QString statement, int return_values);

    /*!
     * \brief Checks if an entry exists in the database, based on position data
     */
    bool exists(AEntry entry);

    /*!
     * \brief commits an entry to the database, calls either insert or update,
     * based on position data
     */
    bool commit(AEntry entry);

    /*!
     * \brief Create new entry in the databse based on UserInput
     */
    bool insert(AEntry new_entry);

    /*!
     * \brief Updates entry in database from existing entry tweaked by the user.
     */
    bool update(AEntry updated_entry);

    /*!
     * \brief deletes an entry from the database.
     */
    bool remove(AEntry entry);

    /*!
     * \brief retreive entry data from the database to create an entry object
     */
    TableData getEntryData(DataPosition data_position);

    /*!
     * \brief retreive an Entry from the database.
     */
    AEntry getEntry(DataPosition data_position);

    /*!
     * \brief retreives a PilotEntry from the database.
     *
     * This function is a wrapper for DataBase::getEntry(DataPosition),
     * where the table is already set and which returns a PilotEntry
     * instead of an Entry. It allows for easy access to a pilot entry
     * with only the RowId required as input.
     */
    APilotEntry getPilotEntry(RowId row_id);
    // [G] TODO: Ensure PilotDialog works great and slowly move to
    // other dialogs

    /*!
     * \brief getCompletionList returns a QStringList of values for a
     * QCompleter based on database values
     * \return
     */
    QStringList getCompletionList(CompleterTarget);
signals:
    void sqlSuccessful();

    void sqlError(const QSqlError &sqlError, const QString &sqlStatement);

};

/*!
 * \brief Convinience function that returns instance of DataBase.
 * Instead of this:
 * DataBase::getInstance().commit(...)
 * Write this:
 * aDB()->commit(...)
 */
ADataBase* aDB();

}  // namespace experimental

#endif
