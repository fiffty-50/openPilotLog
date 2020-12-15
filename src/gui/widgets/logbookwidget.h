/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
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
#ifndef LOGBOOKWIDGET_H
#define LOGBOOKWIDGET_H

#include <QWidget>
#include <QItemSelection>
#include <QSqlTableModel>
#include <QMessageBox>
#include <chrono>
#include <QDebug>
#include <QMenu>
#include <QTableView>

#include "src/classes/asettings.h"
#include "src/database/db.h"
#include "src/classes/flight.h"
#include "src/gui/dialogues/newflightdialog.h"
#include "src/experimental/aflightentry.h"

namespace Ui {
class LogbookWidget;
}

class LogbookWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogbookWidget(QWidget *parent = nullptr);
    ~LogbookWidget();

private slots:
    void on_newFlightButton_clicked();

    void on_editFlightButton_clicked();

    void on_deleteFlightPushButton_clicked();

    void on_showAllButton_clicked();

    void flightsTableView_selectionChanged();

    void on_tableView_customContextMenuRequested(const QPoint &pos);

    void on_actionDelete_Flight_triggered();

    void on_actionEdit_Flight_triggered();

    void on_tableView_doubleClicked();

    void on_flightSearchLlineEdit_textChanged(const QString &arg1);

    void on_flightSearchComboBox_currentIndexChanged();

private:
    Ui::LogbookWidget *ui;

    QTableView* view;

    QSqlTableModel* displayModel;

    QItemSelectionModel* selection;

    QMenu* menu;

    QMessageBox* nope;

    QVector<qint32> selectedFlights;

    void prepareModelAndView(int view_id);

    void setupDefaultView();

    void setupEasaView();

    void connectSignalsAndSlots();


};

#endif // LOGBOOKWIDGET_H
