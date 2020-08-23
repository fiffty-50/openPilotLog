#include "logbookwidget.h"
#include "ui_logbookwidget.h"
#include <QSqlTableModel>
#include <QMessageBox>
#include "dbman.cpp"
#include "newflight.h"
#include "editflight.h"

#include <chrono>
#include <QDebug>

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
    view->hideColumn(0); // don't show the ID
    view->show();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    qDebug() << "homeWidget: Time taken for lookup and rendering: " << duration.count() << " microseconds";
}

logbookWidget::~logbookWidget()
{
    delete ui;
}

void logbookWidget::on_newFlightButton_clicked()
{
    NewFlight nf(this);
    nf.exec();
}

void logbookWidget::on_editFlightButton_clicked()
{
    EditFlight ef(this);
    ef.exec();
}

void logbookWidget::on_deleteFlightPushButton_clicked()
{
    if(SelectedFlight > 0)
    {
        qDebug() << "Valid Flight Selected";
        // To Do: Confirmation Dialog
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

void logbookWidget::on_tableView_entered(const QModelIndex &index)
{
    auto NewIndex = ui->tableView->model()->index(index.row(), 0);
    SelectedFlight = ui->tableView->model()->data(NewIndex).toInt();
    qDebug() << "Selected Flight with ID#(entered): " << SelectedFlight;
}

void logbookWidget::on_tableView_pressed(const QModelIndex &index)
{
    auto NewIndex = ui->tableView->model()->index(index.row(), 0);
    SelectedFlight = ui->tableView->model()->data(NewIndex).toInt();
    qDebug() << "Selected Flight with ID#(entered): " << SelectedFlight;
}
