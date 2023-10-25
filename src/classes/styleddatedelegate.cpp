#include "styleddatedelegate.h"

StyledDateDelegate::StyledDateDelegate(OPL::Date::Format dateFormat, QObject *parent)
    :
    QStyledItemDelegate(parent),
    m_dateFormat(dateFormat)
{}

QString StyledDateDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    const OPL::Date date(value.toInt());
    return date.toString(m_dateFormat);
}
