#include "styledtimedelegate.h"
#include "src/classes/time.h"

StyledTimeDelegate::StyledTimeDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{}

QString StyledTimeDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    const OPL::Time time(value.toInt());
    return time.toString();
}
