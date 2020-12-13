#include "aentry.h"

namespace experimental {

AEntry::AEntry(DataPosition position_)
    : position(position_)
{}

AEntry::AEntry(TableData table_data)
    : tableData(table_data)
{}

AEntry::AEntry(DataPosition position_, TableData table_data)
    : position(position_), tableData(table_data)
{}

void AEntry::setData(TableData table_data)
{
    tableData = table_data;
}

const DataPosition& AEntry::getPosition()
{
    return position;
}

const TableData& AEntry::getData()
{
    return tableData;
}

}  // namespace experimental
