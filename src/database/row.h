/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
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
 * referred to as entries. See AircraftEntry, FlightEntry etc.
 */
class Row
{
public:
    Row() { valid = false;} // Require specifying position
    Row(OPL::DbTable table_name, int row_id, const RowData_T &row_data);
    Row(OPL::DbTable table_name, int row_id);
    Row(OPL::DbTable table_name);

    Row(const Row&) = default;
    Row& operator=(const Row&) = default;

    const RowData_T& getData() const;
    void setData(const RowData_T &value);
    int getRowId() const;
    void setRowId(int value);
    OPL::DbTable getTable() const;
    const QString getTableName() const;
    const QString getPosition() const;

    bool isValid() const {return hasData && valid;}

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

/*!
 * \brief A Row representing an Aircraft entry. See Row class for details.
 */
class AircraftEntry : public Row
{
public:
    AircraftEntry();
    AircraftEntry(const RowData_T &row_data);
    AircraftEntry(int row_id, const RowData_T &row_data);
};

/*!
 * \brief A Row representing a Tail (Registration) entry. See Row class for details.
 */
class TailEntry : public Row
{
public:
    TailEntry();
    TailEntry(const RowData_T &row_data);
    TailEntry(int row_id, const RowData_T &row_data);

    const QString registration() const { return getData().value(OPL::Db::TAILS_REGISTRATION).toString(); }
    const QString type()         const { return getData().value(OPL::Db::TAILS_MAKE).toString(); } //TODO - Create String for make-model-variant
};

/*!
 * \brief A Row representing a Pilot entry. See Row class for details.
 */
class PilotEntry : public Row
{
public:
    PilotEntry();
    PilotEntry(const RowData_T &row_data);
    PilotEntry(int row_id, const RowData_T &row_data);

    const QString lastName()  const { return getData().value(OPL::Db::PILOTS_LASTNAME).toString(); }
    const QString firstName() const { return getData().value(OPL::Db::PILOTS_FIRSTNAME).toString(); }

};

/*!
 * \brief A Row representing a Simulator entry. See Row class for details.
 */
class SimulatorEntry : public Row
{
public:
    SimulatorEntry();
    SimulatorEntry(const RowData_T &row_data);
    SimulatorEntry(int row_id, const RowData_T &row_data);
};

/*!
 * \brief A Row representing a Flight entry. See Row class for details.
 */
class FlightEntry : public Row
{
public:
    FlightEntry();
    FlightEntry(const RowData_T &row_data);
    FlightEntry(int row_id, const RowData_T &row_data);
};

/*!
 * \brief A Row representing a Currency entry. See Row class for details.
 */
class CurrencyEntry : public Row
{
public:
    CurrencyEntry();
    CurrencyEntry(const RowData_T &row_data);
    CurrencyEntry(int row_id, const RowData_T &row_data);
};

/*!
 * \brief A Row representing an Airport entry. See Row class for details.
 */
class AirportEntry : public Row
{
public:
    AirportEntry();
    AirportEntry(const RowData_T &row_data);
    AirportEntry(int row_id, const RowData_T &row_data);

    const QString iata() const { return getData().value(OPL::Db::AIRPORTS_IATA).toString(); }
    const QString icao() const { return getData().value(OPL::Db::AIRPORTS_ICAO).toString(); }
};

} // namespace OPL
#endif // ROW_H
