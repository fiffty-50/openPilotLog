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
#ifndef LOGBOOKWIDGETUI_H
#define LOGBOOKWIDGETUI_H

#include "src/gui/dialogues/entryeditdialog.h"
#include <QComboBox>
#include <QGridLayout>
#include <QPushButton>
#include <QSqlTableModel>
#include <QStackedWidget>
#include <QTableView>

namespace Ui {

class LogbookWidgetUi {
  public:
    void setupUi(QWidget *parent = nullptr);
    void retranslateUi();

    QGridLayout *mainGridLayout;
    QGridLayout *buttonGridLayout;
    QSqlTableModel *model;
    QTableView *view;
    // QWidget *filterWidget;
    QWidget *buttonWidget;
    QPushButton *newFlightButton;
    QPushButton *newSimButton;
    QPushButton *deleteButton;
    QStackedWidget *stackedWidget;
    // QLineEdit *filterLineEdit;
    // QComboBox *filterSelectionComboBox;

    EntryEditDialog *flightEntryEditDialog;
    EntryEditDialog *simEntryEditDialog;

  private:
    void createLayout(QWidget *parent = nullptr);
    void setupButtonWidget(QWidget *parent = nullptr);
};

} // namespace Ui

#endif // LOGBOOKWIDGETUI_H
