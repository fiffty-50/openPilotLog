#ifndef STYLEDREGISTRATIONDELEGATE_H
#define STYLEDREGISTRATIONDELEGATE_H

#include <QStyledItemDelegate>

/*!
 * \brief The StyledRegistrationDelegate class is used to display a database date value human-readable.
 * \details The database stores tails as an integer representing a foreign key into a database
 * of tails. This delegate uses the Database cache to map this ID to a registration to display
 * in the view.
 */
class StyledRegistrationDelegate : public QStyledItemDelegate
{
public:
    explicit StyledRegistrationDelegate(QObject *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // STYLEDREGISTRATIONDELEGATE_H
