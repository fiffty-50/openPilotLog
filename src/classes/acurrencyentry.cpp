#include "acurrencyentry.h"
#include "src/opl.h"

AEntry::AEntry(DataPosition position_)
    : position(position_)
{}

AEntry::AEntry(RowData_T table_data)
    : tableData(table_data)
{}

AEntry::AEntry(DataPosition position_, RowData_T table_data)
    : position(position_), tableData(table_data)
{}

void AEntry::setData(RowData_T table_data)
{
    tableData = table_data;
}

void AEntry::setPosition(DataPosition position_)
{
    position = position_;
}

const DataPosition& AEntry::getPosition() const
{
    return position;
}

const RowData_T& AEntry::getData() const
{
    return tableData;
}

AEntry::operator QString() const
{
    QString out("\033[32m[Entry Data]:\t\033[m\n");
    int item_count = 0;
    QHash<ColName_T, ColData_T>::const_iterator i;
    for (i = tableData.constBegin(); i!= tableData.constEnd(); ++i) {
        QString spacer(":");
        int spaces = (14 - i.key().length());
        if (spaces > 0)
            for (int i = 0; i < spaces ; i++)
                spacer += QLatin1Char(' ');
        if (i.value().toString().isEmpty()) {
            out.append(QLatin1String("\t\033[m") + i.key()
                       + spacer
                       + QLatin1String("\033[35m----"));
            spaces = (14 - i.value().toString().length());
            spacer = QString();
            if (spaces > 0)
                for (int i = 0; i < spaces ; i++)
                    spacer += QLatin1Char(' ');
            out.append(spacer);
        } else {
            out.append(QLatin1String("\t\033[m") + i.key()
                       + spacer
                       + QLatin1String("\033[35m")
                       + i.value().toString());

            spaces = (14 - i.value().toString().length());
            spacer = QString();
            if (spaces > 0)
                for (int i = 0; i < spaces ; i++)
                    spacer += QLatin1Char(' ');
            out.append(spacer);
        }
        item_count ++;
        if (item_count % 4 == 0)
            out.append(QLatin1String("\n"));
    }
    out.append(QLatin1String("\n"));
    QTextStream(stdout) << out;
    return QString();
}
/*!
 * \brief ACurrencyEntry::ACurrencyEntry Creates an ACurrenyEntry object.
 *
 * The Data Position is initalized by using the strongly typed enum
 * CurrencyName, which maps to the static row id for the currency.
 */
ACurrencyEntry::ACurrencyEntry(ACurrencyEntry::CurrencyName currency_name)
    : AEntry::AEntry(DataPosition(OPL::Db::TABLE_CURRENCIES, static_cast<int>(currency_name)))
{}

ACurrencyEntry::ACurrencyEntry(ACurrencyEntry::CurrencyName currency_name, QDate expiration_date)
    : AEntry::AEntry(DataPosition(OPL::Db::TABLE_CURRENCIES, static_cast<int>(currency_name)))
{
    if (expiration_date.isValid()) {
        tableData.insert(OPL::Db::CURRENCIES_EXPIRYDATE, expiration_date.toString(Qt::ISODate));
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
                tableData.value(OPL::Db::CURRENCIES_EXPIRYDATE).toString(), Qt::ISODate
                ).isValid();
}
