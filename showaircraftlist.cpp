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
#include "showaircraftlist.h"
#include "ui_showaircraftlist.h"


ShowAircraftList::ShowAircraftList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowAircraftList)
{
    ui->setupUi(this);

    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("ViewAircraftList");
    model->select();


    QTableView *view = ui->tableView;
    view->setModel(model);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    //view->hideColumn(0); // don't show the ID
    view->show();
}

ShowAircraftList::~ShowAircraftList()
{
    delete ui;
}

void ShowAircraftList::on_closeButton_clicked()
{
    QDialog::close();
}
