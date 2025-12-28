#ifndef ENTRYBUILDER_H
#define ENTRYBUILDER_H
#include "src/database/row.h"

class EntryBuilder
{
public:
    virtual ~EntryBuilder() = default;

    virtual OPL::Row getEntry() const = 0;
    virtual bool isValid() const = 0;
    virtual QStringList invalidFields() const = 0;

protected:
    EntryBuilder() = default;
};

#endif // ENTRYBUILDER_H
