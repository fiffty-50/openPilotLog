#include "acurrencyentry.h"
#include "src/opl.h"

//ACurrencyEntry::ACurrencyEntry()
//{
//
//}
/*!
 * \brief ACurrencyEntry::ACurrencyEntry Creates an ACurrenyEntry object.
 *
 * The Data Position is initalized by using the strongly typed enum
 * CurrencyName, which maps to the static row id for the currency.
 */
ACurrencyEntry::ACurrencyEntry(ACurrencyEntry::CurrencyName currency_name)
    : AEntry::AEntry(DataPosition(Opl::Db::TABLE_CURRENCIES, static_cast<int>(currency_name)))
{}

ACurrencyEntry::ACurrencyEntry(ACurrencyEntry::CurrencyName currency_name, QDate expiration_date)
    : AEntry::AEntry(DataPosition(Opl::Db::TABLE_CURRENCIES, static_cast<int>(currency_name)))
{
    if (expiration_date.isValid()) {
        tableData.insert(Opl::Db::CURRENCIES_EXPIRYDATE, expiration_date.toString(Qt::ISODate));
    } else {
        DEB << "Invalid Date.";
    }
}

/*!
 * \brief ACurrencyEntry::isValid returns true if the object holds a valid expiration date.
 *
 * Unless a user has previously entered an expiration date, this will return false.
 * \return
 */
bool ACurrencyEntry::isValid() const
{
    return QDate::fromString(
                tableData.value(Opl::Db::CURRENCIES_EXPIRYDATE).toString(), Qt::ISODate
                ).isValid();
}
