#ifndef ENTRY_H
#define ENTRY_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QPair>

#include "Decl.h"

namespace experimental {

/*!
 * \brief The Entry class encapsulates table metadata(table name, row id)
 *  and data for new and existing entries in the database to operate on.
 */
class Entry {
public:
    DataPosition position;
protected:
    TableData tableData;
public:
    Entry() = default;
    Entry(const Entry&) = default;
    void operator=(const Entry&);
    Entry(DataPosition position_);
    void setData(TableData table_data);
    const TableData& getData();

};

class PilotEntry : public Entry {
public:
    PilotEntry() = default;
    PilotEntry(const PilotEntry& pe);
    void operator=(const PilotEntry& pe);
    PilotEntry(int row_id);
    PilotEntry(TableData fromNewPilotDialog);
};

}

#endif // ENTRY_H
