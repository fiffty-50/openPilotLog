#ifndef ACURRENCYENTRY_H
#define ACURRENCYENTRY_H

#include "src/database/adatabasetypes.h"

/*!
 * \brief The Entry class encapsulates table metadata(table name, row id)
 *  and data for new and existing entries in the database to operate on.
 */
class AEntry {
protected:
    DataPosition position;
public:
    RowData_T tableData;
public:
    AEntry() = delete;
    AEntry(const AEntry&) = default;
    AEntry& operator=(const AEntry&) = default;
    AEntry(DataPosition position_);
    AEntry(RowData_T table_data);
    AEntry(DataPosition position_, RowData_T table_data);

    void setData(RowData_T table_data);
    void setPosition(DataPosition position_);

    const DataPosition& getPosition() const;
    const TableName_T &getTableName() const { return position.tableName; }
    const RowId_T &getRowId() const { return position.rowId; }
    const RowData_T& getData() const;

    /*!
     * \brief operator QString provides compatibilty with QDebug() - prints
     * the tableData in a readable formatting to stdout
     */
    operator QString() const;

};

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
