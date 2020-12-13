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
#ifndef AIRCRAFTWIDGET_H
#define AIRCRAFTWIDGET_H

#include <QWidget>
#include <QItemSelection>
#include <QSqlTableModel>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>

#include "src/classes/asettings.h"
#include "src/gui/dialogues/newtaildialog.h"
#include "src/classes/aircraft.h"
#include "src/database/db.h"


namespace Ui {
class AircraftWidget;
}

class AircraftWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AircraftWidget(QWidget *parent = nullptr);
    ~AircraftWidget();

private slots:
    void tableView_selectionChanged();

    void tableView_headerClicked(int column);

    void on_deleteButton_clicked();

    void on_newButton_clicked();

    void acft_editing_finished();

    void on_searchLineEdit_textChanged(const QString &arg1);

private:
    Ui::AircraftWidget *ui;

    QSqlTableModel *model = new QSqlTableModel;

    qint32 sortColumn;

    QVector<qint32> selectedTails;

    void refreshModelAndView();
};

#endif // AIRCRAFTWIDGET_H
