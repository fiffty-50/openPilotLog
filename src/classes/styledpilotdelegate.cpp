#include "styledpilotdelegate.h"
#include "src/database/databasecache.h"

StyledPilotDelegate::StyledPilotDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QString StyledPilotDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    return DBCache->getPilotNamesMap().value(value.toInt());
}
