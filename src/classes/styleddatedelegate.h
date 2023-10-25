#ifndef STYLEDDATEDELEGATE_H
#define STYLEDDATEDELEGATE_H

#include <QStyledItemDelegate>
#include "src/classes/date.h"

/*!
 * \brief The StyledDateDelegate class is used to display a database date value human-readable.
 * \details The database stores dates as an integer representing the days elapsed since the
 * beginning of the julian calendar. This integer has to be converted to a human-readable date
 * according to the users selected date format.
 */
class StyledDateDelegate : public QStyledItemDelegate
{
public:
    StyledDateDelegate(OPL::Date::Format dateFormat, QObject * parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;
private:
    OPL::Date::Format m_dateFormat;
};

#endif // STYLEDDATEDELEGATE_H
