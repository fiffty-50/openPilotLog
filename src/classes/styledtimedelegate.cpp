#include "styledtimedelegate.h"
#include "src/classes/time.h"

StyledTimeDelegate::StyledTimeDelegate(const OPL::DateTimeFormat &format, QObject *parent)
    : QStyledItemDelegate{parent}, m_format(format)
{}

QString StyledTimeDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    const OPL::Time time(value.toInt(), m_format);
    return time.toString();
}
