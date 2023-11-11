#ifndef STYLEDDATEDELEGATE_H
#define STYLEDDATEDELEGATE_H

#include <QStyledItemDelegate>
#include "src/opl.h"

/*!
 * \brief The StyledDateDelegate class is used to display a database date value human-readable.
 * \details The database stores dates as an integer representing the days elapsed since the
 * beginning of the julian calendar. This integer has to be converted to a human-readable date
 * according to the users selected date format.
 */
class StyledDateDelegate : public QStyledItemDelegate
{
public:
    StyledDateDelegate(const OPL::DateTimeFormat &dateFormat, QObject * parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;
private:
    OPL::DateTimeFormat m_format;
};

#endif // STYLEDDATEDELEGATE_H
