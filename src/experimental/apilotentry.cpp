#include "apilotentry.h"

namespace experimental {

APilotEntry::APilotEntry()
    : AEntry::AEntry(DEFAULT_PILOT_POSITION)
{}

APilotEntry::APilotEntry(int row_id)
    : AEntry::AEntry(DataPosition("pilots", row_id))
{}

APilotEntry::APilotEntry(TableData table_data)
    : AEntry::AEntry(DEFAULT_PILOT_POSITION, table_data)
{}

} // namespace experimental
