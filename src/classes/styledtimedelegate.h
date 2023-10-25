#ifndef STYLEDTIMEDELEGATE_H
#define STYLEDTIMEDELEGATE_H

#include <QStyledItemDelegate>

/*!
 * \brief The StyledTimeDelegate class is used to convert the database time format to a human-readable format.
 * \details The database stores time values as an integer representing minutes elapsed since midnight. This
 * delegate can be used in a QTableView to format the database value as "hh:mm"
 */
class StyledTimeDelegate : public QStyledItemDelegate
{
public:
    explicit StyledTimeDelegate(QObject *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // STYLEDTIMEDELEGATE_H
