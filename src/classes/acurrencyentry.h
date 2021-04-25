#ifndef ACURRENCYENTRY_H
#define ACURRENCYENTRY_H

#include "src/classes/aentry.h"
#include "src/database/adatabasetypes.h"

struct ACurrencyEntry : public AEntry
{
public:
    enum class CurrencyName {
        Licence     = 1,
        TypeRating  = 2,
        LineCheck   = 3,
        Medical     = 4,
        Custom1     = 5,
        Custom2     = 6
    };

    ACurrencyEntry() = delete;
    ACurrencyEntry(CurrencyName name);
    ACurrencyEntry(CurrencyName name, QDate expiration_date);

    ACurrencyEntry(const ACurrencyEntry& te) = default;
    ACurrencyEntry& operator=(const ACurrencyEntry& te) = default;

    bool isValid() const;
};

#endif // ACURRENCYENTRY_H
