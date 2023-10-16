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
 * \details The Row class is a base class and when instantiated, the appropriate subclass should be used.
 *
 * The database holds all the data related to the logbook in different tables. Each of these tables is composed of
 * rows. Each row has different columns and each column contains the data. As such, an entry can be thought of
 * as a row in the database. The row class encapsulates the data contained in each row.
 *
 * A row is uniquely identified by its position in the database, consisting of the table name (QString) and the row id (int).
 * A new entry, which is not yet in the database has the row id 0. If a new row object is created, the hasData
 * bool is set to false. Before submitting the entry to the database, setData() has to be called to fill the row
 * with data and toggle the verification bit.
 *
 * The Row Object holds all the necessary information the Database class needs to commit (create or update) it.
 * The Identifying information can be accessed with getRowId and getTable() / getTableName().
 *
 * For convenience and readabilty, subclasses exist that have the table property pre-set. These rows are then
 * referred to as entries. See AircraftEntry, FlightEntry etc. These subclasses have public static members which
 * hold the column names used in the sql database. These can be used to access the data held in the row by column.
 */
class Row
{
public:

    /*!
     * \brief Create a new empty row entry
     */
    Row();
    /*!
     * \brief Create a row entry specifying its table, row id and row data.
     */
    Row(OPL::DbTable table_name, int row_id, const RowData_T &row_data);
    /*!
     * \brief Create a row entry specifying its table and row id.
     */
    Row(OPL::DbTable table_name, int row_id);

    /*!
     * \brief Create a row entry specifying its table name.
     * \param table_name
     */
    Row(OPL::DbTable table_name);

    Row(const Row&) = default;
    Row& operator=(const Row&) = default;

    /*!
     * \brief get the Row Data contained in the Row
     * \details The row data is a Map where the sql column name is the key and its value is the value.
     */
    const RowData_T& getData() const;

    void setData(const RowData_T &value);

    /*!
     * \brief Get the entries row id in the database
     */
    int getRowId() const;

    /*!
     * \brief Set the entries row id in the database
     */
    void setRowId(int value);

    OPL::DbTable getTable() const;

    /*!
     * \brief returns a string representation of the entries position in the database (Table and Row ID)
     */
    const QString getPosition() const;

    /*!
     * \brief get the name of the table in the sql database.
     * \details This method has to be overwritten in any subclass to return the table name, this should be
     * a purely virtual function but in order to be able to use row class instances this function is implemented
     * to return an empty strring in the base class.
     * \return The name of the table in the database containing a valid row, or an empty String
     */
    virtual const QString getTableName() const;

    /*!
     * \brief A Row entry is valid if its table and row are specified and if it contains row data.
     */
    bool isValid() const;

    /*!
     * \brief operator QString can be used for printing debug information to stdout
     */
    operator QString() const;


private:
    OPL::DbTable table;
    int rowId;
    RowData_T rowData;
protected:
    bool hasData;
    bool valid = true;
};

} // namespace OPL
#endif // ROW_H
