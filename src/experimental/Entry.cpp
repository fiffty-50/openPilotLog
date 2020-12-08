#include "Entry.h"

namespace experimental {

Entry::Entry(DataPosition position_)
    : position(position_)
{}

void Entry::operator=(const Entry& other)
{
    position = other.position;
    tableData = other.tableData;
}

void Entry::setData(TableData table_data)
{
    tableData = table_data;
}

const TableData& Entry::getData()
{
    return tableData;
}

PilotEntry::PilotEntry(const PilotEntry& pe)
    : Entry::Entry(pe)
{}

void PilotEntry::operator=(const PilotEntry& pe)
{
    position = pe.position;
    tableData = pe.tableData;
}

PilotEntry::PilotEntry(int row_id)
    : Entry::Entry(DataPosition("pilots", row_id))
{}

PilotEntry::PilotEntry(TableData fromNewPilotDialog)
    : Entry::Entry(DEFAULT_PILOT_POSITION)
{
    setData(fromNewPilotDialog);
}

}  // namespace experimental
