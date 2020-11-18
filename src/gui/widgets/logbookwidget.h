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

#include "src/classes/settings.h"
#include "src/database/db.h"
#include "src/classes/flight.h"
#include "src/gui/dialogues/newflight.h"

namespace Ui {
class LogbookWidget;
}



class LogbookWidget : public QWidget
{
    Q_OBJECT


public:
    explicit LogbookWidget(QWidget *parent = nullptr);
    ~LogbookWidget();

    qint32 selectedFlight = 0;
    void setSelectedFlight(const qint32 &value);

private slots:
    void on_newFlightButton_clicked();

    void on_editFlightButton_clicked();

    void on_deleteFlightPushButton_clicked();

    void on_filterFlightsByDateButton_clicked();

    void on_showAllButton_clicked();

    void tableView_selectionChanged(const QItemSelection &index, const QItemSelection &);

private:
    Ui::LogbookWidget *ui;

    void refreshView(int view_id);

    void defaultView();

    void easaView();

};

#endif // LOGBOOKWIDGET_H
