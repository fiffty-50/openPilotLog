#ifndef STYLEDTYPEDELEGATE_H
#define STYLEDTYPEDELEGATE_H

#include <QStyledItemDelegate>
#include <QObject>

class StyledTypeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit StyledTypeDelegate(QObject *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // STYLEDTYPEDELEGATE_H
