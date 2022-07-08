#ifndef AIRCRAFTENTRY_H
#define AIRCRAFTENTRY_H

#include "row.h"

namespace OPL {

class AircraftEntry : public Row
{
public:
    AircraftEntry();
    AircraftEntry(const RowData_T &row_data);
    AircraftEntry(int row_id, const RowData_T &row_data);
};

} // namespace OPL


#endif // AIRCRAFTENTRY_H
