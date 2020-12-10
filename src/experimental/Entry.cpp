#include "Entry.h"

namespace experimental {

Entry::Entry(DataPosition position_)
    : position(position_)
{
//    position = position_;
}
/// [F] from what I understand the initialiser list should do exactly the same as
///     the body, but for some reason it doesn't. Example:
///     auto entry = DB()->getPilotEntry(7);
///     DEB(entry.position); // returns {"pilots",7}
///
///     PilotEntry copiedEntry;
///     copiedEntry = entry;
///     DEB(copiedEntry.position);
///     returns {"pilots",0} with initalizer list
///     returns {"pilots", 7} with constructor as above.
///
///     I think I'm missing something but can't figure it out.

void Entry::setData(TableData table_data)
{
    tableData = table_data;
}

const TableData& Entry::getData()
{
    return tableData;
}

PilotEntry::PilotEntry(int row_id)
{
    position = DataPosition("pilots", row_id);
}

PilotEntry::PilotEntry(TableData fromNewPilotDialog)
    : Entry::Entry(DEFAULT_PILOT_POSITION)
{
    setData(fromNewPilotDialog);
}

}  // namespace experimental
