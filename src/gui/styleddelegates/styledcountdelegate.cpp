#include "styledcountdelegate.h"

StyledCountDelegate::StyledCountDelegate(QObject *parent) : QStyledItemDelegate{parent} {}

QString StyledCountDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    if(value.toInt() == 0) return {};
    else return QString::number(value.toInt());
}
