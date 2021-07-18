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
}; // namespace aDbSetup

#endif // ADBSETUP_H
