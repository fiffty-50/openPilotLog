#ifndef AAIRCRAFTENTRY_H
#define AAIRCRAFTENTRY_H

#include "src/experimental/aentry.h"
#include "src/experimental/decl.h"

namespace experimental {

struct AAircraftEntry : public AEntry {
public:
    AAircraftEntry();
    AAircraftEntry(const AAircraftEntry& te) = default;
    AAircraftEntry& operator=(const AAircraftEntry& te) = default;
    AAircraftEntry(int row_id);
    AAircraftEntry(TableData table_data);
};

} // namespace experimental

#endif // AAIRCRAFTENTRY_H
