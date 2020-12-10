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
    Entry& operator=(const Entry&) = default;
    Entry(DataPosition position_);
    void setData(TableData table_data);
    const TableData& getData();

};

class PilotEntry : public Entry {
public:
    PilotEntry() = default;
    PilotEntry(const PilotEntry& pe) = default;
    PilotEntry& operator=(const PilotEntry& pe) = default;
    PilotEntry(int row_id);
    PilotEntry(TableData newPilotData);
};

}

#endif // ENTRY_H
