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
#include "src/testing/adebug.h"

LogbookWidget::LogbookWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogbookWidget)
{
    ui->setupUi(this);
    ui->filterDateEdit->setDate(QDate::currentDate());
    ui->filterDateEdit_2->setDate(QDate::currentDate());
    ui->newFlightButton->setFocus();

    //customContextMenu for tablewidget
    menu  = new QMenu(this);
    menu->addAction(ui->actionEdit_Flight);
    menu->addAction(ui->actionDelete_Flight);

    //message Box
    nope = new QMessageBox(this);

    refreshView(ASettings::read("logbook/view").toInt());
}

LogbookWidget::~LogbookWidget()
{
    delete ui;
}

/*
 * Functions
 */

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
    QTableView *view = ui->tableView;
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->hideColumn(0);
    connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(tableView_selectionChanged()));
}

void LogbookWidget::defaultView()
{
    DEB("Loading Default View...");
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("viewDefault");
    model->select();

    QTableView *view = ui->tableView;
    view->setModel(model);

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

    view->show();
}

void LogbookWidget::easaView()
{
    DEB("Loading EASA View...");
    QSqlTableModel *model = new QSqlTableModel;
    model->setTable("viewEASA");
    model->select();

    QTableView *view = ui->tableView;
    view->setModel(model);

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
}

/*
 * Slots
 */

void LogbookWidget::tableView_selectionChanged()//
{
    auto *selection = ui->tableView->selectionModel();

    selectedFlights.clear();
    for (const auto& row : selection->selectedRows()) {
        selectedFlights << row.data().toInt();
        DEB("Selected Flight(s) with ID: " << selectedFlights);
    }
}

void LogbookWidget::on_newFlightButton_clicked()
{
    auto nf = new NewFlightDialog(this, Db::createNew);
    nf->setAttribute(Qt::WA_DeleteOnClose);
    nf->exec();
    refreshView(ASettings::read("logbook/view").toInt());
}

void LogbookWidget::on_editFlightButton_clicked()
{
    if(selectedFlights.length() == 1){
        auto ef = new NewFlightDialog(this,Flight(selectedFlights.first()), Db::editExisting);
        ef->setAttribute(Qt::WA_DeleteOnClose);
        ef->exec();
        refreshView(ASettings::read("logbook/view").toInt());
    } else if (selectedFlights.isEmpty()) {
        nope->setText("No flight selected.\n");
        nope->exec();
    } else {
        nope->setText("More than one flight selected.\n\nEditing multiple entries is not yet supported.");
        nope->exec();
    }
}

void LogbookWidget::on_deleteFlightPushButton_clicked()
{
    DEB("Flights selected: " << selectedFlights.length());
    if (selectedFlights.length() > 0 && selectedFlights.length() < 11) {
        QVector<QString> columns = {
            "doft", "dept", "dest"
        };
        QVector<QString> details;
        QString warningMsg = "The following flight(s) will be deleted:<br><br><b><tt>";
        for(const auto& selectedFlight : selectedFlights){
            details = Db::multiSelect(columns, "flights", "flight_id",
                                       QString::number(selectedFlight), Db::exactMatch);
            for (const auto &item : details) {
                warningMsg.append(item);
                warningMsg.append(' ');
            }
            warningMsg.append("<br>");
        }
        warningMsg.append("</b></tt><br>Deleting Flights is irreversible."
                          "<br>Do you want to proceed?");

        QMessageBox confirm;
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.setIcon(QMessageBox::Question);
        confirm.setWindowTitle("Delete Flight");
        confirm.setText(warningMsg);
        int reply = confirm.exec();
        if (reply == QMessageBox::Yes) {
            for (const auto& selectedFlight : selectedFlights) {
                DEB("Deleting flight with ID# " << selectedFlight);
                auto entry = Flight(selectedFlight);
                entry.remove();
            }
            refreshView(ASettings::read("logbook/view").toInt());
        }
    } else if (selectedFlights.length() == 0) {
        nope->setIcon(QMessageBox::Information);
        nope->setText("No Flight Selected.");
        nope->exec();
    } else if (selectedFlights.length() > 10) {
        auto& warningMsg = "You have selected " + QString::number(selectedFlights.length())
                         + " flights.\n\n Deleting these flights is irreversible.\n\n"
                           "Are you sure you want to proceed?";
        QMessageBox confirm;
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.setIcon(QMessageBox::Warning);
        confirm.setWindowTitle("Delete Flight");
        confirm.setText(warningMsg);
        int reply = confirm.exec();
        if(reply == QMessageBox::Yes) {
            for (const auto& selectedFlight : selectedFlights) {
                DEB("Deleting flight with ID# " << selectedFlight);
                auto entry = Flight(selectedFlight);
                entry.remove();
            }
            refreshView(ASettings::read("logbook/view").toInt());
        }
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
    refreshView(ASettings::read("logbook/view").toInt());
}

void LogbookWidget::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void LogbookWidget::on_actionDelete_Flight_triggered()
{
    emit ui->deleteFlightPushButton->clicked();
}

void LogbookWidget::on_actionEdit_Flight_triggered()
{
    emit ui->editFlightButton->clicked();
}

void LogbookWidget::on_tableView_doubleClicked()
{
    emit ui->editFlightButton->clicked();
}
