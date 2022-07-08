#include "aircraftentry.h"

namespace OPL {

AircraftEntry::AircraftEntry()
    : Row(DbTable::Aircraft, 0)
{}

OPL::AircraftEntry::AircraftEntry(const RowData_T &row_data)
    : Row(DbTable::Aircraft, 0, row_data)
{}

OPL::AircraftEntry::AircraftEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Aircraft, row_id, row_data)
{}

} // namespace OPL
