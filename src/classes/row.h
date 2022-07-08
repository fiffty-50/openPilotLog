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

    RowData_T getRowData() const;
    void setRowData(const RowData_T &value);
    int getRowId() const;
    void setRowId(int value);
    OPL::DbTable getTable() const;

    bool isValid() const {return hasData && valid;}

    // For Debugging
    operator QString() const;


private:
    OPL::DbTable table;
    int rowId;
    RowData_T rowData;
    bool hasData;
    bool valid = true;
};

} // namespace OPL
#endif // ROW_H
