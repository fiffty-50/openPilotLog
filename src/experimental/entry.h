#ifndef ENTRY_H
#define ENTRY_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QPair>

#include "Decl.h"

namespace experimental {

/// [G]: Define what data is public and what not. For objects such as
/// DataPosition which are consumable its no biggy. Are entries the same?
/// If so we could avoid getters and setters
/*!
 * \brief The Entry class encapsulates table metadata(table name, row id)
 *  and data for new and existing entries in the database to operate on.
 */
class Entry {
protected:
    DataPosition position;
    TableData tableData;
public:
    Entry() = delete; // Demand specificity from default constructor
    Entry(const Entry&) = default;
    Entry& operator=(const Entry&) = default;
    Entry(DataPosition position_);
    Entry(TableData table_data);
    Entry(DataPosition position_, TableData table_data);

    void setData(TableData table_data);
    void setPosition(DataPosition position_);

    const DataPosition& getPosition();
    const TableData& getData();

};

struct PilotEntry : public Entry {
public:
    PilotEntry();
    PilotEntry(const PilotEntry& pe) = default;
    PilotEntry& operator=(const PilotEntry& pe) = default;
    PilotEntry(int row_id);
    PilotEntry(TableData table_data);
};

}

#endif // ENTRY_H
