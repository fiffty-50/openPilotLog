#ifndef APILOTENTRY_H
#define APILOTENTRY_H

#include "src/experimental/aentry.h"
#include "src/experimental/decl.h"

namespace experimental {

struct APilotEntry : public AEntry {
public:
    APilotEntry();
    APilotEntry(const APilotEntry& pe) = default;
    APilotEntry& operator=(const APilotEntry& pe) = default;
    APilotEntry(int row_id);
    APilotEntry(TableData table_data);
};

} // namespace experimental

#endif // APILOTENTRY_H
