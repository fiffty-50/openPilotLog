#ifndef STYLEDCOUNTDELEGATE_H
#define STYLEDCOUNTDELEGATE_H

#include <QStyledItemDelegate>

class StyledCountDelegate : public QStyledItemDelegate {
  public:
    explicit StyledCountDelegate(QObject *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // STYLEDCOUNTDELEGATE_H
