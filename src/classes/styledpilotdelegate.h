#ifndef STYLEDPILOTDELEGATE_H
#define STYLEDPILOTDELEGATE_H

#include <QStyledItemDelegate>

/*!
 * \brief The StyledPilotDelegate class is used to display a database date value human-readable.
 * \details The database stores pilots as an integer representing a foreign key into a database
 * of pilots. This delegate uses the Database cache to map this ID to a name and displays the name
 * in the view.
 */
class StyledPilotDelegate : public QStyledItemDelegate
{
public:
    explicit StyledPilotDelegate(QObject *parent = nullptr);

    QString displayText(const QVariant &value, const QLocale &locale) const override;
};

#endif // STYLEDPILOTDELEGATE_H
