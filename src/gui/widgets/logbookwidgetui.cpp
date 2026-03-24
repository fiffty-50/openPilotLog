/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
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
#include "logbookwidgetui.h"

namespace Ui {

void LogbookWidgetUi::setupUi(QWidget *parent)
{
    createLayout(parent);
    retranslateUi();
    QWidget::setTabOrder({view, newFlightButton, newSimButton, deleteButton});
    stackedWidget->hide();
}

void LogbookWidgetUi::retranslateUi()
{
    newFlightButton->setText(QObject::tr("New Flight"));
    newSimButton->setText(QObject::tr("New Simulator Session"));
    deleteButton->setText(QObject::tr("Delete Selected Entry"));
    //filterLabel->setText(QObject::tr("Search"));
}

void LogbookWidgetUi::createLayout(QWidget *parent)
{
    // create a single column grid layout and fill the cells
    constexpr int col = 0;
    int row           = 0;
    mainGridLayout    = new QGridLayout(parent);

    view = new QTableView(parent);
    mainGridLayout->addWidget(view, row, col);
    row++;

    stackedWidget = new QStackedWidget(parent);
    mainGridLayout->addWidget(stackedWidget, row, col);
    row++;

    setupButtonWidget(parent);
    mainGridLayout->addWidget(buttonWidget);
    row++;

    // stackedWidget->addWidget(filterWidget);
    // stackedWidget->setCurrentWidget(filterWidget);
}

void LogbookWidgetUi::setupButtonWidget(QWidget *parent)
{
    buttonWidget            = new QWidget(parent);
    buttonGridLayout        = new QGridLayout(buttonWidget);
    newFlightButton         = new QPushButton(parent);
    newSimButton            = new QPushButton(parent);
    deleteButton            = new QPushButton(parent);
    // filterLabel             = new QLabel(parent);
    // filterLineEdit          = new QLineEdit(parent);
    // filterSelectionComboBox = new QComboBox(parent);

    buttonGridLayout->addWidget(newFlightButton, 0, 0);
    buttonGridLayout->addWidget(newSimButton, 1, 0);
    buttonGridLayout->addWidget(deleteButton, 2, 0);


    // place the filter items in a grid layout so they occupy one cell in parent layout
    // filterWidget     = new QWidget(parent);
    // filterWidgetLayout = new QGridLayout(filterWidget);

    // // one row, three columns
    // filterWidgetLayout->addWidget(filterLabel);
    // filterWidgetLayout->addWidget(filterLineEdit, 0, 1);
    // filterWidgetLayout->addWidget(filterSelectionComboBox, 0, 2);

    // buttonGridLayout->addWidget(filterWidget, 3, 0);
}

} // namespace Ui
