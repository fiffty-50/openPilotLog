/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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

#ifndef ROW_H
#define ROW_H
#include "src/opl.h"

namespace OPL {

/*!
 * \brief The Row class provides an interface for retreiving and submitting entries from the database.
 *
 * \details The Row class is an abstract class.
 *
 * The database holds all the data related to the logbook in different tables. Each of these tables is composed of
 * rows. Each row has different columns and each column contains the data. As such, an entry can be thought of
 * as a row in the database. The row class encapsulates the data contained in each row.
 *
 * A Row is uniquely identified by its position in the database, consisting of the table name (QString) and the row id (int).
 * A new entry, which is not yet in the database has the row id 0. if a new row object is created setData() has to be called
 * before submitting the entry to the database.
 *
 * The Row Object holds all the necessary information the Database class needs to commit (create or update) it.
 * The Identifying information can be accessed with getRowId() and getTable() or getTableName().
 *
 * Row is an abstract class. Derived classes exists for each table in the database that needs to be accessed from
 * the application. These subclasses 
 * <ul>
 *      <li> Must provide a List<QString> with the names of the fields in the table </li>
 *      <li> Implement the isValid() method to check if the row contains valid data for the table </li>
 * </ul>
 *
 */
class Row
{
protected:
    Row() = delete;
    /*!
     * \brief Create a row entry specifying its table, row id and row data.
     */
    explicit Row(OPL::DbTable table_name, int row_id, const RowData_T &row_data, const QList<QString> *fields);
    /*!
     * \brief Create a row entry specifying its table name.
     * \param table_name The name of the table in the database
     * \param fields A list of all fields in the table, except the rowId
     */
    explicit Row(OPL::DbTable table_name, const QList<QString> *fields);
public:

    Row(const Row&) = default;
    Row& operator=(const Row&) = default;

    /*!
     * \brief get the Row Data contained in the Row
     * \details The row data is a QHash<QString, QVariant> where the sql column name is the key and its value is the value.
     */
    const RowData_T& getData() const;

    /*!
     * \brief set the Row Data contained in the Row
    */
    void setData(const RowData_T &value);

    /*!
     * \brief Get the entries row id in the database
     */
    int getRowId() const;

    /*!
     * \brief Set the entries row id in the database
     */
    void setRowId(int value);

    /*!
     * \brief Get the table in the database containing the row
    */
    OPL::DbTable getTable() const;

    /*!
     * \brief returns a string representation of the entries position in the database (Table and Row ID)
     */
    const QString getPosition() const;

    /*!
     * \brief get the name of the table in the sql database as a QString
     */
    QString getTableName() const { return OPL::GLOBALS->getDbTableName(m_table); }

    /*!
     * \brief A Row entry is valid if its table and row are specified and if it contains semantically correct data.
     */
    virtual bool isValid() const = 0;

    /*!
     * \brief operator QString can be used for printing debug information to stdout
     */
    operator QString() const;

protected:
    OPL::DbTable m_table;
    int m_rowId;
    RowData_T m_rowData;
    const QStringList *m_fields;
};

} // namespace OPL
#endif // ROW_H
