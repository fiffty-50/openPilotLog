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

namespace Ui {
class HomeWidget;
}

class HomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);
    ~HomeWidget();

private:
    Ui::HomeWidget *ui;

    QDate today;
    int currWarningThreshold;
    double ftlWarningThreshold;

    void fillTotals();
    void fillCurrencies();
    void fillCurrencyTakeOffLanding();
    void fillLimitations();



    QList<QLabel*> limitationDisplayLabels;
    /*!
     * \brief Retreives the users first name from the database.
     */
    const QString userName();

    enum class Colour {Red, Orange};

    inline void setLabelColour(QLabel* label, Colour colour)
    {
        switch (colour) {
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
    void fillCurrency(ASettings::UserData date, QLabel *display_label);
public slots:
    void onHomeWidget_dataBaseUpdated();
};

#endif // HOMEWIDGET_H
