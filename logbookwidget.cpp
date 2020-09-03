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
#include "logbookwidget.h"
#include "ui_logbookwidget.h"
#include <QSqlTableModel>
#include <QMessageBox>
#include "dbman.cpp"
#include "dbflight.h"
#include "newflight.h"
#include "editflight.h"

#include <chrono>
#include <QDebug>

//To Do: Update Selection in Tableview on arrow key press.

qint32 SelectedFlight = -1; // Variable to store selected row in QTableView

logbookWidget::logbookWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::logbookWidget)
{
    ui->setupUi(this);
    ui->filterDateEdit->setDate(QDate::currentDate());

    auto start = std::chrono::high_resolution_clock::now(); // timer for performance testing

    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("Logbook");
    model->select();

    QTableView *view = ui->tableView;
    view->setModel(model);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setColumnWidth(1,120);
    view->setColumnWidth(2,60);
    view->setColumnWidth(3,60);
    view->setColumnWidth(4,60);
    view->setColumnWidth(5,60);
    view->setColumnWidth(6,60);
    view->setColumnWidth(7,120);
    view->setColumnWidth(8,180);
    view->setColumnWidth(9,120);
    view->setColumnWidth(10,90);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->hideColumn(0);
    view->show();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    qDebug() << "logbookWidget: Time taken for lookup and rendering: " << duration.count() << " microseconds";

    connect(ui->tableView->selectionModel(),
    SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
    SLOT(tableView_selectionChanged(const QItemSelection &, const QItemSelection &)));
}

void logbookWidget::tableView_selectionChanged(const QItemSelection &index, const QItemSelection &)// TO DO
{
    SelectedFlight = index.indexes()[0].data().toInt();
    qDebug() << "Selected Flight with ID#(selectionChanged): " << SelectedFlight;
}

logbookWidget::~logbookWidget()
{
    delete ui;
}

void logbookWidget::on_newFlightButton_clicked()
{
    //NewFlight nf(this);
    //nf.exec();
}

void logbookWidget::on_editFlightButton_clicked() // To Do: Fix! - use new flight, pre-filled with entry loaded from DB
{
    QMessageBox *nope = new QMessageBox(this); // edit widget currently INOP
    nope->setText("This feature is temporarily INOP.");
    nope->exec();
    //EditFlight ef(this);
    //ef.exec();
}

void logbookWidget::on_deleteFlightPushButton_clicked()
{
    if(SelectedFlight > 0)
    {
        qDebug() << "Valid Flight Selected";
        // To Do: Confirmation Dialog
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Flight", "The following flight will be deleted:\n{retreive Flight Details}\nAre you sure?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            qDebug() << "Deleting Flight with ID# " << SelectedFlight;
            dbFlight::deleteFlightById(QString::number(SelectedFlight));

            QSqlTableModel *ShowAllModel = new QSqlTableModel; //refresh view
            ShowAllModel->setTable("Logbook");
            ShowAllModel->select();
            ui->tableView->setModel(ShowAllModel);
        }
    }else
    {
        QMessageBox NoFlight;
        NoFlight.setText("No flight selected.");
        NoFlight.exec();
    }
}

void logbookWidget::on_filterFlightsByDateButton_clicked()
{
    QDate selection(ui->filterDateEdit->date());
    QString selecteddate = selection.toString("yyyy-MM-dd");
    QString datefilter = "Date = '" + selecteddate +"'";

    QSqlTableModel *DateFilteredModel = new QSqlTableModel;
    DateFilteredModel ->setTable("Logbook");
    DateFilteredModel ->setFilter(datefilter);
    DateFilteredModel->select();

    ui->tableView->setModel(DateFilteredModel);
}

void logbookWidget::on_showAllButton_clicked()
{
    QSqlTableModel *ShowAllModel = new QSqlTableModel;
    ShowAllModel->setTable("Logbook");
    ShowAllModel->select();

    ui->tableView->setModel(ShowAllModel);
}

/*void logbookWidget::on_tableView_pressed(const QModelIndex &index)
{
    auto NewIndex = ui->tableView->model()->index(index.row(), 0);
    SelectedFlight = ui->tableView->model()->data(NewIndex).toInt();
    qDebug() << "Selected Flight with ID#(pressed): " << SelectedFlight;
}*/
