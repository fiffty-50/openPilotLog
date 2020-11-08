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

// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr

LogbookWidget::LogbookWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogbookWidget)
{
    ui->setupUi(this);
    ui->filterDateEdit->setDate(QDate::currentDate());
    ui->filterDateEdit_2->setDate(QDate::currentDate());
    ui->newFlightButton->setFocus();
    QSettings settings;
    refreshView(settings.value("logbook/view").toInt());
}

LogbookWidget::~LogbookWidget()
{
    delete ui;
}

void LogbookWidget::setSelectedFlight(const qint32 &value)
{
    selectedFlight = value;
}

void LogbookWidget::tableView_selectionChanged(const QItemSelection &index,
                                               const QItemSelection &)// TO DO
{
    setSelectedFlight(index.indexes()[0].data().toInt());
    DEB("Selected flight with ID#: " << selectedFlight);
}

void LogbookWidget::on_newFlightButton_clicked()
{
    //NewFlight nf(this);
    //nf.exec();
    QMessageBox *nope = new QMessageBox(this);
    nope->setText("This feature is temporarily INOP.");
    nope->exec();
}

void LogbookWidget::on_editFlightButton_clicked() // To Do: Fix! - use new flight, pre-filled with entry loaded from DB
{
    QMessageBox *nope = new QMessageBox(this); // edit widget currently INOP
    nope->setText("This feature is temporarily INOP.");
    nope->exec();
    //EditFlight ef(this);
    //ef.exec();
}

void LogbookWidget::on_deleteFlightPushButton_clicked()
{
    if (selectedFlight > 0) {
        QVector<QString> columns = {
            "doft", "dept", "dest"
        };
        QVector<QString> details = Db::multiSelect(columns, "flights", "id",
                                                   QString::number(selectedFlight), Db::exactMatch);
        QString detailsstring = "The following flight will be deleted:\n\n";
        for (const auto &item : details) {
            detailsstring.append(item);
            detailsstring.append(QLatin1Char(' '));
        }
        detailsstring.append("\n\nAre you sure?");

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Flight", detailsstring,
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            DEB("Deleting flight with ID# " << selectedFlight);
            auto en = new Flight(selectedFlight);
            en->remove();
            QSettings settings;
            refreshView(settings.value("logbook/view").toInt());
        }
    } else {
        QMessageBox NoFlight;
        NoFlight.setText("No flight selected.");
        NoFlight.exec();
    }
}

void LogbookWidget::on_filterFlightsByDateButton_clicked()
{
    QDate date(ui->filterDateEdit->date());
    QString startdate = date.toString("yyyy-MM-dd");
    date = ui->filterDateEdit_2->date();
    QString enddate = date.toString("yyyy-MM-dd");
    QString datefilter = "Date BETWEEN '" + startdate + "' AND '" + enddate + QLatin1Char('\'');

    QSqlTableModel *DateFilteredModel = new QSqlTableModel;
    DateFilteredModel ->setTable("Logbook");
    DateFilteredModel ->setFilter(datefilter);
    DateFilteredModel->select();

    ui->tableView->setModel(DateFilteredModel);
}

void LogbookWidget::on_showAllButton_clicked()
{
    QSettings settings;
    refreshView(settings.value("logbook/view").toInt());
    /*QSqlTableModel *ShowAllModel = new QSqlTableModel;
    ShowAllModel->setTable("Logbook");
    ShowAllModel->select();

    ui->tableView->setModel(ShowAllModel);*/
}

void LogbookWidget::refreshView(int view_id)
{
    switch (view_id) {
    case 0:
        defaultView();
        break;
    case 1:
        easaView();
        break;
    default:
        defaultView();
    }

}

void LogbookWidget::defaultView()
{
    DEB("Loading Default View...");
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("Logbook");
    model->select();

    QTableView *view = ui->tableView;
    view->setModel(model);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setColumnWidth(1, 120);
    view->setColumnWidth(2, 60);
    view->setColumnWidth(3, 60);
    view->setColumnWidth(4, 60);
    view->setColumnWidth(5, 60);
    view->setColumnWidth(6, 60);
    view->setColumnWidth(7, 180);
    view->setColumnWidth(8, 180);
    view->setColumnWidth(9, 120);
    view->setColumnWidth(10, 90);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->hideColumn(0);
    view->show();

    connect(ui->tableView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            SLOT(tableView_selectionChanged(const QItemSelection &, const QItemSelection &)));
}

void LogbookWidget::easaView()
{
    DEB("Loading EASA View...");
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("viewEASA");
    model->select();

    QTableView *view = ui->tableView;
    view->setModel(model);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->hideColumn(0);

    view->setColumnWidth(1,120);
    view->setColumnWidth(2,60);
    view->setColumnWidth(3,60);
    view->setColumnWidth(4,60);
    view->setColumnWidth(5,60);
    view->setColumnWidth(6,180);
    view->setColumnWidth(7,120);
    view->setColumnWidth(8,30);
    view->setColumnWidth(9,30);
    view->setColumnWidth(10,30);
    view->setColumnWidth(11,30);
    view->setColumnWidth(12,120);
    view->setColumnWidth(13,15);
    view->setColumnWidth(14,15);
    view->setColumnWidth(15,60);
    view->setColumnWidth(16,60);
    view->setColumnWidth(17,60);
    view->setColumnWidth(18,60);
    view->setColumnWidth(19,60);
    view->setColumnWidth(20,60);
    view->setColumnWidth(21,120);

    view->show();
    connect(ui->tableView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            SLOT(tableView_selectionChanged(const QItemSelection &, const QItemSelection &)));
}

