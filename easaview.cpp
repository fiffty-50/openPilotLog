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
#include "easaview.h"
#include "ui_easaview.h"
#include <QSqlTableModel>

EasaView::EasaView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EasaView)
{
    ui->setupUi(this);


    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("ViewEasa");
    model->select();


    QTableView *view = ui->EasaTableView;
    view->setModel(model);
    view->setAlternatingRowColors(true);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view->verticalHeader()->hide();
    view->hideColumn(0);
    view->show();
}    //model->setEditStrategy(QSqlTableModel::OnManualSubmit);

EasaView::~EasaView()
{
    delete ui;
}

void EasaView::on_QuitButton_clicked()
{
    EasaView::reject();
}
