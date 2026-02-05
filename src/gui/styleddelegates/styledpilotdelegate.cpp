#include "styledpilotdelegate.h"
#include "src/database/cache/pilotinfo.h"

StyledPilotDelegate::StyledPilotDelegate(QObject *parent) : QStyledItemDelegate{parent} {}

QString StyledPilotDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale);
    return pilotsData->name(value.toInt());
}
