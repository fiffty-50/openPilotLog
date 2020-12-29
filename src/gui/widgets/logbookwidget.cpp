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

#include "src/classes/aflightentry.h"
#include "src/database/adatabase.h"
#include "src/classes/asettings.h"
#include "src/gui/dialogues/newflightdialog.h"
#include "src/testing/adebug.h"

const QMap<int, QString> FILTER_MAP = {
    {0, "Date LIKE \"%"},
    {1, "Dept LIKE \"%"},
    {2, "Dest LIKE \"%"},
    {3, "Registration LIKE \"%"},
    {4, "\"Name PIC\" LIKE \"%"}
};
const auto NON_WORD_CHAR = QRegularExpression("\\W");

LogbookWidget::LogbookWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogbookWidget)
{
    ui->setupUi(this);
    ui->newFlightButton->setFocus();

    //customContextMenu for tablewidget
    menu  = new QMenu(this);
    menu->addAction(ui->actionEdit_Flight);
    menu->addAction(ui->actionDelete_Flight);

    //Initialise message Box
    messageBox = new QMessageBox(this);

    prepareModelAndView(ASettings::read("logbook/view").toInt());
    connectSignalsAndSlots();
}

LogbookWidget::~LogbookWidget()
{
    delete ui;
}

/*
 * Functions
 */

void LogbookWidget::prepareModelAndView(int view_id)
{
    switch (view_id) {
    case 0:
        setupDefaultView();
        break;
    case 1:
        setupEasaView();
        break;
    default:
        setupDefaultView();
    }
}

void LogbookWidget::connectSignalsAndSlots()
{
    selectionModel = view->selectionModel();
    QObject::connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &LogbookWidget::flightsTableView_selectionChanged);
}

void LogbookWidget::setupDefaultView()
{
    DEB "Loading Default View...";
    displayModel = new QSqlTableModel;
    displayModel->setTable("viewDefault");
    displayModel->select();

    view = ui->tableView;
    view->setModel(displayModel);

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

    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->hideColumn(0);

    view->show();
}

void LogbookWidget::setupEasaView()
{
    DEB "Loading EASA View...";
    displayModel = new QSqlTableModel;
    displayModel->setTable("viewEASA");
    displayModel->select();

    view = ui->tableView;
    view->setModel(displayModel);

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

    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->hideColumn(0);

    view->show();
}

/*
 * Slots
 */

void LogbookWidget::flightsTableView_selectionChanged()//
{
    selectedFlights.clear();
    for (const auto& row : selectionModel->selectedRows()) {
        selectedFlights.append(row.data().toInt());
        DEB "Selected Flight(s) with ID: " << selectedFlights;
    }
}

void LogbookWidget::on_newFlightButton_clicked()
{
    auto nf = new NewFlightDialog(this);
    nf->setAttribute(Qt::WA_DeleteOnClose);
    nf->exec();
    displayModel->select();
}

void LogbookWidget::on_editFlightButton_clicked()
{
    if(selectedFlights.length() == 1){
        auto ef = new NewFlightDialog(selectedFlights.first(), this);
        ef->setAttribute(Qt::WA_DeleteOnClose);
        ef->exec();
        displayModel->select();
    } else if (selectedFlights.isEmpty()) {
        messageBox->setText("No flight selected.\n");
        messageBox->exec();
    } else {
        messageBox->setText("More than one flight selected.\n\nEditing multiple entries is not yet supported.");
        messageBox->exec();
    }
}

void LogbookWidget::on_deleteFlightPushButton_clicked()
{
    DEB "Flights selected: " << selectedFlights.length();
    if (selectedFlights.length() == 0) {
        messageBox->setIcon(QMessageBox::Information);
        messageBox->setText("No Flight Selected.");
        messageBox->exec();
        return;
    } else if (selectedFlights.length() > 0 && selectedFlights.length() < 11) {
        QList<AFlightEntry> flights_list;

        for (const auto &flight_id : selectedFlights) {
            flights_list.append(aDB()->getFlightEntry(flight_id));
        }

        QString warningMsg = "The following flight(s) will be deleted:<br><br><b><tt>";

        for (auto &flight : flights_list) {
            warningMsg.append(flight.summary());
            warningMsg.append(QLatin1String("&nbsp;&nbsp;&nbsp;&nbsp;<br>"));
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
            for (auto& flight : flights_list) {
                DEB "Deleting flight: " << flight.summary();
                if(!aDB()->remove(flight)) {
                    messageBox->setText(aDB()->lastError.text());
                    messageBox->exec();
                    return;
                }
            }
            messageBox->setText(QString::number(selectedFlights.length()) + " flights have been deleted successfully.");
            messageBox->exec();
            displayModel->select();
        }
    } else if (selectedFlights.length() > 10) {
        auto& warningMsg = "You have selected " + QString::number(selectedFlights.length())
                + " flights.\n\n Deleting flights is irreversible.\n\n"
                  "Are you sure you want to proceed?";
        QMessageBox confirm;
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.setIcon(QMessageBox::Warning);
        confirm.setWindowTitle("Delete Flight");
        confirm.setText(warningMsg);
        int reply = confirm.exec();
        if(reply == QMessageBox::Yes) {
            QList<DataPosition> selected_flights;
            for (const auto& flight_id : selectedFlights) {
                selected_flights.append({QLatin1String("flights"), flight_id});
            }
            if (!aDB()->removeMany(selected_flights)) {

                messageBox->setText(aDB()->lastError.text()); // [F]: To Do: error info
                messageBox->exec();
                return;
            }
            messageBox->setText(QString::number(selectedFlights.length()) + " flights have been deleted successfully.");
            messageBox->exec();
            displayModel->select();
        }
        displayModel->select();
    }
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

void LogbookWidget::on_flightSearchComboBox_currentIndexChanged(int)
{
    emit ui->showAllButton->clicked();
}

void LogbookWidget::onDisplayModel_dataBaseUpdated()
{
    //refresh view to reflect changes the user has made via a dialog.
    displayModel->select();
}

void LogbookWidget::onLogbookWidget_viewSelectionChanged(int view_id)
{
    prepareModelAndView(view_id);
}

void LogbookWidget::on_showAllButton_clicked()
{
    ui->flightSearchLlineEdit->setText(QString());
    displayModel->setFilter(QString());
    displayModel->select();
}

void LogbookWidget::on_flightSearchLlineEdit_textChanged(const QString &arg1)
{
    if(arg1.length() == 0) {
        displayModel->setFilter("");
        displayModel->select();
        return;
    }

    if (ui->flightSearchComboBox->currentIndex() < 3) {
        displayModel->setFilter(FILTER_MAP.value(ui->flightSearchComboBox->currentIndex())
                                + arg1 + "%\"");
        return;
    } else if (ui->flightSearchComboBox->currentIndex() == 3) { // registration
        displayModel->setFilter(FILTER_MAP.value(ui->flightSearchComboBox->currentIndex())
                                + arg1 + "%\"");
        return;
    } else if (ui->flightSearchComboBox->currentIndex() == 4) { // Name Pic
        displayModel->setFilter(FILTER_MAP.value(ui->flightSearchComboBox->currentIndex())
                                + arg1 + "%\"");
        return;
    }
}
