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
#ifndef PILOTSWIDGET_H
#define PILOTSWIDGET_H

#include <QWidget>
#include <QItemSelection>
#include <QSqlTableModel>
#include <QDebug>
#include <QLabel>
#include <QSettings>
#include "src/classes/pilot.h"
#include "src/gui/dialogues/newpilot.h"

namespace Ui {
class PilotsWidget;
}

class PilotsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PilotsWidget(QWidget *parent = nullptr);
    ~PilotsWidget();

    void setSelectedPilot(const qint32 &value);

private slots:
    void tableView_selectionChanged(const QItemSelection &index, const QItemSelection &);

    void on_newButton_clicked();

    void on_deletePushButton_clicked();

    void on_widget_accepted();

private:
    Ui::PilotsWidget *ui;

    qint32 selectedPilot = 0;

    void refreshView();
};

#endif // PILOTSWIDGET_H
