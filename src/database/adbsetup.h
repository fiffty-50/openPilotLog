#ifndef ADBSETUP_H
#define ADBSETUP_H

#include <QCoreApplication>

/*!
 * \brief The aDbSetup namespace is responsible for the inital setup of the database when
 * the application is first launched. It creates the database in the specified default
 * location and creates all required tables and views. It can also be used to reset the
 * database currently used
 */
namespace aDbSetup
{
bool createDatabase();

/*!
 * \brief createSchemata creates the required tables for the database
 * \return true on success
 */
bool createSchemata(const QStringList &statements);

/*!
 * \brief commitData commits the data read from a JSON file into a table in the database.
 */
bool commitData(const QJsonArray &json_arr, const QString &table_name);

/*!
 * \brief clearUserData Deletes all data from user tables. Resets the database.
 */
void clearUserData();

/*!
 * \brief importTemplateData fills an empty database with the template
 * data (Aircraft, Airports, currencies, changelog) as read from the JSON
 * templates
 * \param use_local_ressources determines whether the included ressource files
 * or a previously downloaded file should be used.
 * \return
 */
bool importTemplateData(bool use_local_ressources);
}; // namespace aDbSetup

#endif // ADBSETUP_H
