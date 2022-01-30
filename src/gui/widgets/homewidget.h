/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include "src/functions/astat.h"
#include "src/database/adatabase.h"
#include "src/classes/asettings.h"
#include "src/classes/acurrencyentry.h"

namespace Ui {
class HomeWidget;
}

/*!
 * \brief The HomeWidget is the welcome screen of the application.
 * \details The HomeWidget shows total flight times and a user-configurable set of currencies
 * (expiry dates for licenses, ratings, medicals,...). Most data is provided by the AStat class
 * and the ACurrencyEntry class. Notifications are provided by means of pop-up warnings on application
 * start via QMessageBox and the INFO/WARN interfaces, as well as by colouring the labels according
 * to the warning level (orange/red).
 */
class HomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);
    ~HomeWidget();

private:
    Ui::HomeWidget *ui;

    QList<QLabel*> limitationDisplayLabels;
    QDate          today;
    /*!
     * \brief currWarningThreshold - Retreived from ASettings::UserData::CurrWarningThreshold, the number
     * of days before expiry that the user gets notified about impending expiries.
     */
    int currWarningThreshold;
    /*!
     * \brief ftlWarningThreshold - Retreived from ASettings::UserData::FtlWarningThreshold, the percentage
     * of how close the user has to be to reaching a Flight Time Limitation before getting notified.
     */
    double ftlWarningThreshold;

    void fillTotals();
    void fillSelectedCurrencies();
    void fillCurrencyTakeOffLanding();
    void fillCurrency(ACurrencyEntry::CurrencyName currency_name, QLabel *display_label);
    void fillLimitations();

    enum class Colour {Red, Orange, None};
    inline void setLabelColour(QLabel* label, Colour colour)
    {
        switch (colour) {
        case Colour::None:
            label->setStyleSheet(QString());
            break;
        case Colour::Red:
            label->setStyleSheet(QStringLiteral("color: red"));
            break;
        case Colour::Orange:
            label->setStyleSheet(QStringLiteral("color: orange"));
            break;
        default:
            label->setStyleSheet(QString());
            break;
        }
    }

    inline void hideLabels(QLabel* label1, QLabel* label2) {
        label1->hide();
        label2->hide();
    }

    /*!
     * \brief Retreives the users first name from the database.
     */
    const QString userName();

public slots:
    void refresh();

protected:
    /*!
     * \brief Handles change events, like updating the UI to new localisation
     */
    void changeEvent(QEvent* event) override;
};

#endif // HOMEWIDGET_H
