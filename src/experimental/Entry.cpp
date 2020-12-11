#include "Entry.h"

namespace experimental {

Entry::Entry(DataPosition position_)
    : position(position_)
{}

Entry::Entry(TableData table_data)
    : tableData(table_data)
{}

Entry::Entry(DataPosition position_, TableData table_data)
    : position(position_), tableData(table_data)
{}

void Entry::setData(TableData table_data)
{
    tableData = table_data;
}

const DataPosition& Entry::getPosition()
{
    return position;
}

const TableData& Entry::getData()
{
    return tableData;
}

PilotEntry::PilotEntry()
    : Entry::Entry(DEFAULT_PILOT_POSITION)
{}

PilotEntry::PilotEntry(int row_id)
    : Entry::Entry(DataPosition("pilots", row_id))
{}

PilotEntry::PilotEntry(TableData table_data)
    : Entry::Entry(DEFAULT_PILOT_POSITION, table_data)
{}

}  // namespace experimental
