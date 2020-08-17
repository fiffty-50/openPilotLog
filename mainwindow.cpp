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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "newflight.h"
#include "editflight.h"
#include "newacft.h"
#include "easaview.h"
#include "dbman.cpp"
#include "calc.h"
#include <QTime>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTableView>
#include <chrono>
#include <QMessageBox>

qlonglong SelectedFlight = -1;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);



    db::connect();
    ui->FilterDateEdit->setDate(QDate::currentDate());


    auto start = std::chrono::high_resolution_clock::now();
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
    //view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->hideColumn(0); // don't show the ID
    view->show();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    qDebug() << "Time taken for lookup and rendering: " << duration.count() << " microseconds";

}

MainWindow::~MainWindow()
{
    delete ui;
}
/*
 * Functions
 */

void MainWindow::nope()
{
    QMessageBox nope; //error box
    nope.setText("This feature is not yet available!");
    nope.exec();
}

/*
 * Slots
 */

void MainWindow::on_newflightButton_clicked()
{
    NewFlight nf(this);
    nf.exec();
}


void MainWindow::on_actionNew_Flight_triggered()
{
    NewFlight nf(this);
    nf.exec();
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_quitButton_clicked()
{
    QApplication::quit();
}


void MainWindow::on_ShowAllButton_clicked()
{
    QSqlTableModel *ShowAllModel = new QSqlTableModel;
    ShowAllModel->setTable("Logbook");
    ShowAllModel->select();

    ui->tableView->setModel(ShowAllModel);
}

void MainWindow::on_filterComboBox_activated(const QString &arg1)
{
    if (arg1 == "Pilot Name")
    {
        nope();
    }else if (arg1 == "Aircraft Registration")
    {
        nope();
    }else if (arg1 == "Airline")
    {
        nope();
    }else if (arg1 == "Multi-Engine")
    {
        nope();
    }
}

void MainWindow::on_editflightButton_clicked()
{
    QVector<QString> flight;
    flight = db::SelectFlightById(QString::number(SelectedFlight));
    if(flight.length() > 0)
    {
        qDebug() << "Valid Flight Selected";
        db::CommitToScratchpad(flight); // commits flight to scratchpad
        EditFlight ef(this);
        ef.exec();
    }

}

void MainWindow::on_deleteFlightPushButton_clicked()
{
    if(SelectedFlight > 0)
    {
        qDebug() << "Valid Flight Selected";
        db::DeleteFlightById(QString::number(SelectedFlight));



        QSqlTableModel *ShowAllModel = new QSqlTableModel; //refresh view
        ShowAllModel->setTable("Logbook");
        ShowAllModel->select();

        ui->tableView->setModel(ShowAllModel);
    }else
    {
        QMessageBox NoFlight;
        NoFlight.setText("No flight selected.");
        NoFlight.exec();
    }
}

void MainWindow::on_FilterDateEdit_editingFinished()
{

}

void MainWindow::on_filterFlightsByDateButton_clicked()
{
    QDate selection(ui->FilterDateEdit->date());
    QString selecteddate = selection.toString("yyyy-MM-dd");
    QString datefilter = "Date = '" + selecteddate +"'";

    QSqlTableModel *DateFilteredModel = new QSqlTableModel;
    DateFilteredModel ->setTable("Logbook");
    DateFilteredModel ->setFilter(datefilter);
    DateFilteredModel->select();

    ui->tableView->setModel(DateFilteredModel);
}



void MainWindow::on_EasaViewButton_clicked()
{
    EasaView ev(this);
    ev.exec();
}

void MainWindow::on_tableView_pressed(const QModelIndex &index)
{
    auto NewIndex = ui->tableView->model()->index(index.row(), 0);
    SelectedFlight = ui->tableView->model()->data(NewIndex).toInt();
    qDebug() << "Selected Flight with ID#:(presssed) " << SelectedFlight;
}

void MainWindow::on_tableView_entered(const QModelIndex &index)
{
    auto NewIndex = ui->tableView->model()->index(index.row(), 0);
    SelectedFlight = ui->tableView->model()->data(NewIndex).toInt();
    qDebug() << "Selected Flight with ID#(entered): " << SelectedFlight;
}
