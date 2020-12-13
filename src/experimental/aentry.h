#ifndef ENTRY_H
#define ENTRY_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QPair>

#include "decl.h"

namespace experimental {

/// [G]: Define what data is public and what not. For objects such as
/// DataPosition which are consumable its no biggy. Are entries the same?
/// If so we could avoid getters and setters
/*!
 * \brief The Entry class encapsulates table metadata(table name, row id)
 *  and data for new and existing entries in the database to operate on.
 */
class AEntry {
protected:
    DataPosition position;
    TableData tableData;
public:
    AEntry() = delete; // Demand specificity from default constructor
    AEntry(const AEntry&) = default;
    AEntry& operator=(const AEntry&) = default;
    AEntry(DataPosition position_);
    AEntry(TableData table_data);
    AEntry(DataPosition position_, TableData table_data);

    void setData(TableData table_data);
    void setPosition(DataPosition position_);

    const DataPosition& getPosition();
    const TableData& getData();

};

}

#endif // ENTRY_H
