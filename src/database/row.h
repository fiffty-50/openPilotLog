#ifndef ROW_H
#define ROW_H
#include "src/opl.h"


namespace OPL {

/*!
 * \brief The Row class provides an interface for retreiving and submitting entries from the database.
 * It is a bass class and when instantiated, the appropriate subclass should be used.
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

    RowData_T getData() const;
    void setData(const RowData_T &value);
    int getRowId() const;
    void setRowId(int value);
    OPL::DbTable getTableName() const;

    bool isValid() const {return hasData && valid;}

    // For Debugging
    operator QString() const;


private:
    OPL::DbTable table;
    int rowId;
    RowData_T rowData;
protected:
    bool hasData;
    bool valid = true;
};

class AircraftEntry : public Row
{
public:
    AircraftEntry();
    AircraftEntry(const RowData_T &row_data);
    AircraftEntry(int row_id, const RowData_T &row_data);
};

class TailEntry : public Row
{
public:
    TailEntry();
    TailEntry(const RowData_T &row_data);
    TailEntry(int row_id, const RowData_T &row_data);

    const QString registration() const;
    const QString type() const;
};

class PilotEntry : public Row
{
public:
    PilotEntry();
    PilotEntry(const RowData_T &row_data);
    PilotEntry(int row_id, const RowData_T &row_data);

};

class SimulatorEntry : public Row
{
public:
    SimulatorEntry();
    SimulatorEntry(const RowData_T &row_data);
    SimulatorEntry(int row_id, const RowData_T &row_data);
};

class FlightEntry : public Row
{
public:
    FlightEntry();
    FlightEntry(const RowData_T &row_data);
    FlightEntry(int row_id, const RowData_T &row_data);
};

class CurrencyEntry : public Row
{
public:
    CurrencyEntry();
    CurrencyEntry(const RowData_T &row_data);
    CurrencyEntry(int row_id, const RowData_T &row_data);
};

} // namespace OPL
#endif // ROW_H
