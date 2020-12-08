#include "Entry.h"

namespace experimental {

Entry::Entry(QPair<QString, int> position_)
    : position(position_)
{}

void Entry::operator=(const Entry& e)
{
    position = e.position;
    tableData = e.tableData;
}

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

void PilotEntry::operator=(const PilotEntry& pe)
{
    position = pe.position;
    tableData = pe.tableData;
}

PilotEntry::PilotEntry(int row_id)
    : Entry::Entry(QPair<QString, int>("pilots", row_id))
{}

PilotEntry::PilotEntry(QMap<QString, QString> fromNewPilotDialog)
    : Entry::Entry(DEFAULT_PILOT_POSITION)
{
    setData(fromNewPilotDialog);
}

}  // namespace experimental
