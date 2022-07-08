#ifndef TAILENTRY_H
#define TAILENTRY_H

#include "row.h"

namespace OPL {

class TailEntry : public Row
{
public:
    TailEntry();
    TailEntry(const RowData_T &row_data); // create a new entry from scratch
    TailEntry(int row_id, const RowData_T &row_data); // create an existing entry (retreived from DB)

    const QString registration() const;
    const QString type() const;
};

}



#endif // TAILENTRY_H
