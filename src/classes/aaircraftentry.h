#ifndef AAIRCRAFTENTRY_H
#define AAIRCRAFTENTRY_H

#include "src/classes/aentry.h"
#include "src/database/declarations.h"

struct AAircraftEntry : public AEntry {
public:
    AAircraftEntry();
    AAircraftEntry(const AAircraftEntry& te) = default;
    AAircraftEntry& operator=(const AAircraftEntry& te) = default;
    AAircraftEntry(int row_id);
    AAircraftEntry(RowData table_data);
};

#endif // AAIRCRAFTENTRY_H
