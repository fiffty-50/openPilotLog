#include "styledtypedelegate.h"
#include "src/database/cache/aircrafttypesinfo.h"

StyledTypeDelegate::StyledTypeDelegate(QObject *parent) : QStyledItemDelegate{parent} {}

QString StyledTypeDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale);
    return aircraftTypesData->typeString(value.toInt());
}
