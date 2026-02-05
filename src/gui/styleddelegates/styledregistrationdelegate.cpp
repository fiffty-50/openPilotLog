#include "styledregistrationdelegate.h"
#include "src/database/cache/tailregistrationsinfo.h"

StyledRegistrationDelegate::StyledRegistrationDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
}

QString StyledRegistrationDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    Q_UNUSED(locale);
    return tailsData->registration(value.toInt());
}
