#include "styledtypedelegate.h"
#include "src/database/databasecache.h"

StyledTypeDelegate::StyledTypeDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QString StyledTypeDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale);
    return DBCache->getTypesMap().value(value.toInt());
}
