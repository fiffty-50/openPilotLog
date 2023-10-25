#include "styledregistrationdelegate.h"
#include "src/database/databasecache.h"

StyledRegistrationDelegate::StyledRegistrationDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QString StyledRegistrationDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    return DBCache->getTailsMap().value(value.toInt());

}
