#include "styleddatedelegate.h"
#include "src/classes/date.h"

StyledDateDelegate::StyledDateDelegate(const OPL::DateTimeFormat &dateFormat, QObject *parent)
    :
    QStyledItemDelegate(parent),
    m_format(dateFormat)
{}

QString StyledDateDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    return OPL::Date(value.toInt(), m_format).toString();
}
