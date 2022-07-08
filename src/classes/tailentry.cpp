#include "tailentry.h"

namespace OPL {

TailEntry::TailEntry()
    : Row(DbTable::Tails, 0)
{}

TailEntry::TailEntry(const RowData_T &row_data)
    : Row(DbTable::Tails, 0, row_data)
{}

TailEntry::TailEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Tails, row_id, row_data)
{}

const QString TailEntry::registration() const
{
    return getRowData().value(OPL::Db::TAILS_REGISTRATION).toString();
}

const QString TailEntry::type() const
{
    QString type_string;
    if (!getRowData().value(OPL::Db::TAILS_MAKE).toString().isEmpty())
        type_string.append(getRowData().value(OPL::Db::TAILS_MAKE).toString() + QLatin1Char(' '));
    if (!getRowData().value(OPL::Db::TAILS_MODEL).toString().isEmpty())
        type_string.append(getRowData().value(OPL::Db::TAILS_MODEL).toString());
    if (!getRowData().value(OPL::Db::TAILS_VARIANT).toString().isEmpty())
        type_string.append(QLatin1Char('-') + getRowData().value(OPL::Db::TAILS_VARIANT).toString());

    return type_string;
}

} // namespace OPL


