#include "Entry.h"

namespace experimental {

Entry::Entry(DataPosition position_)
    : position(position_)
{}

void Entry::setData(QMap<QString, QString> data)
{
    tableData = data;
}

const QMap<QString, QString>& Entry::getData()
{
    return tableData;
}

PilotEntry::PilotEntry(const PilotEntry& pe)
    : Entry::Entry(pe)
{}

PilotEntry::PilotEntry(int row_id)
    : Entry::Entry(DataPosition("pilots", row_id))
{}

PilotEntry::PilotEntry(QMap<QString, QString> fromNewPilotDialog)
    : Entry::Entry(DEFAULT_PILOT_POSITION)
{
    setData(fromNewPilotDialog);
}

}  // namespace experimental
