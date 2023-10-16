/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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
#include "src/classes/time.h"
#include "src/database/database.h"
#include "logbookwidget.h"
#include "ui_logbookwidget.h"
#include "src/database/row.h"
#include "src/database/database.h"
#include "src/classes/settings.h"
#include "src/gui/dialogues/newflightdialog.h"
#include "src/gui/dialogues/newsimdialog.h"

LogbookWidget::LogbookWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogbookWidget)
{
    ui->setupUi(this);

    OPL::GLOBALS->fillViewNamesComboBox(ui->viewsComboBox);

    //customContextMenu for tablewidget
    menu  = new QMenu(this);
    menu->addAction(ui->actionEdit_Flight);
    menu->addAction(ui->actionDelete_Flight);

    // Initalise the display Model and view
    displayModel = new QSqlTableModel(this);
    view = ui->tableView;

    setupModelAndView(Settings::read(Settings::Main::LogbookView).toInt());
    connectSignalsAndSlots();

}

LogbookWidget::~LogbookWidget()
{
    delete ui;
}

/*
 * Functions
 */

/*!
 * \brief LogbookWidget::setupModelAndView configures the QTableView and populates the model with data
 * according to the current view.
 * \param view_id - retreived from Settings::Main::LogbookView
 */
void LogbookWidget::setupModelAndView(int view_id)
{
    displayModel->setTable(OPL::GLOBALS->getViewIdentifier(OPL::DbViewName(view_id)));
    displayModel->select();

    view->setModel(displayModel);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    view->resizeColumnsToContents();
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->hideColumn(0);
    view->show();
}

void LogbookWidget::connectSignalsAndSlots()
{
    selectionModel = view->selectionModel();
    QObject::connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &LogbookWidget::flightsTableView_selectionChanged);
}

const QString LogbookWidget::getFlightSummary(const OPL::FlightEntry &flight) const
{
    if(!flight.isValid())
        return QString();

    auto tableData = flight.getData();
    QString flight_summary;
    auto space = QLatin1Char(' ');
    flight_summary.append(tableData.value(OPL::FlightEntry::DOFT).toString() + space);
    flight_summary.append(tableData.value(OPL::FlightEntry::DEPT).toString() + space);
    flight_summary.append(OPL::Time(tableData.value(OPL::FlightEntry::TOFB).toInt()).toString()
                          + space);
    flight_summary.append(OPL::Time(tableData.value(OPL::FlightEntry::TONB).toInt()).toString()
                          + space);
    flight_summary.append(tableData.value(OPL::FlightEntry::DEST).toString());

    return flight_summary;
}

void LogbookWidget::changeEvent(QEvent *event)
{
    if (event != nullptr)
        if(event->type() == QEvent::LanguageChange)
            ui->retranslateUi(this);
}

/*!
 * \brief LogbookWidget::flightsTableView_selectionChanged saves the selected row(s)
 * to the selectedFlights member variable.
 */
void LogbookWidget::flightsTableView_selectionChanged()
{
    selectedEntries.clear();
    for (const auto& row : selectionModel->selectedRows()) {
        selectedEntries.append(row.data().toInt());
        DEB << "Selected Flight(s) with ID: " << selectedEntries;
    }
    if (selectedEntries.length() == 1) {
        if (isFlight(selectedEntries.first()))
            on_actionEdit_Flight_triggered();
        else
            on_actionEdit_Sim_triggered();
    }
}

/*!
 * \brief If a row is selected, query information
 * about the affected row(s) and ask the user to confirm deletion.
 */
void LogbookWidget::on_actionDelete_Flight_triggered()
{
    DEB << "Flights selected: " << selectedEntries.length();
    if (selectedEntries.length() == 0) {
        WARN(tr("<br>No flight selected.<br>"));
        return;
    } else if (selectedEntries.length() > 0 && selectedEntries.length() <= 10) {
        QVector<OPL::FlightEntry> flights_list;

        for (const auto &flight_id : qAsConst(selectedEntries)) {
            flights_list.append(DB->getFlightEntry(flight_id));
        }

        QString flights_list_string;

        for (auto &flight : flights_list) {
            flights_list_string.append(getFlightSummary(flight));
            flights_list_string.append(QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;<br>"));
        }

        QMessageBox confirm(this);
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.setIcon(QMessageBox::Question);
        confirm.setWindowTitle("Delete Flight");
        confirm.setText(tr("The following flight(s) will be deleted:<br><br><b><tt>"
                           "%1<br></b></tt>"
                           "Deleting flights is irreversible.<br>Do you want to proceed?"
                           ).arg(flights_list_string));
        if (confirm.exec() == QMessageBox::Yes) {
            for (auto& flight : flights_list) {
                DEB << "Deleting flight: " << flight;
                if(!DB->remove(flight)) {
                    WARN(tr("<br>Unable to delete.<br><br>The following error has ocurred: %1"
                                       ).arg(DB->lastError.text()));
                    return;
                }
            }
            INFO(tr("%1 flights have been deleted successfully."
                                   ).arg(QString::number(selectedEntries.length())));
            displayModel->select();
        }
    } else if (selectedEntries.length() > 10) {
        QMessageBox confirm;
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.setIcon(QMessageBox::Warning);
        confirm.setWindowTitle("Delete Flight");
        confirm.setText(tr("You have selected %1 flights.<br><br>"
                           "Deleting flights is irreversible.<br><br>"
                           "Are you sure you want to proceed?"
                           ).arg(QString::number(selectedEntries.length())));
        if(confirm.exec() == QMessageBox::Yes) {
            if (!DB->removeMany(OPL::DbTable::Flights, selectedEntries)) {
                WARN(tr("Unable to delete. No changes have been made to the database. The following error has ocurred:<br><br>%1").arg(DB->lastError.text()));
                return;
            }
            INFO(tr("%1 flights have been deleted successfully."
                                   ).arg(QString::number(selectedEntries.length())));
            displayModel->select();
        }
        displayModel->select();
    }
}


void LogbookWidget::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    menu->popup(ui->tableView->viewport()->mapToGlobal(pos));
}

void LogbookWidget::on_tableView_doubleClicked()
{
    on_actionEdit_Flight_triggered();
}

void LogbookWidget::on_flightSearchComboBox_currentIndexChanged(int)
{
    //emit ui->showAllButton->clicked();
}

/*!
 * \brief LogbookWidget::refresh Refreshes the view to reflect changes in the database.
 */
void LogbookWidget::refresh()
{
    displayModel->select();
    view->resizeColumnsToContents();
}

void LogbookWidget::onLogbookWidget_viewSelectionChanged(SettingsWidget::SettingSignal signal)
{
    if (signal == SettingsWidget::SettingSignal::LogbookWidget)
        setupModelAndView(Settings::read(Settings::Main::LogbookView).toInt());
}

//void LogbookWidget::on_showAllButton_clicked()
//{
//    ui->flightSearchLlineEdit->setText(QString());
//    displayModel->setFilter(QString());
//    displayModel->select();
//}

/*!
 * \brief LogbookWidget::on_flightSearchLlineEdit_textChanged applies a filter to the
 * display model allowing the user to search for flights by specified elements (date, aircraft,
 * Pilot Name)
 */
void LogbookWidget::on_flightSearchLlineEdit_textChanged(const QString &arg1)
{
    if(arg1.length() == 0) {
        displayModel->setFilter("");
        displayModel->select();
        return;
    }

    if (ui->flightSearchComboBox->currentIndex() < 3) {
        displayModel->setFilter(FILTER_MAP.value(ui->flightSearchComboBox->currentIndex())
                                + arg1 + QLatin1String("%\""));
        return;
    } else if (ui->flightSearchComboBox->currentIndex() == 3) { // registration
        displayModel->setFilter(FILTER_MAP.value(ui->flightSearchComboBox->currentIndex())
                                + arg1 + QLatin1String("%\""));
        return;
    } else if (ui->flightSearchComboBox->currentIndex() == 4) { // Name Pic
        displayModel->setFilter(FILTER_MAP.value(ui->flightSearchComboBox->currentIndex())
                                + arg1 + QLatin1String("%\""));
        return;
    }
}

/*!
 * \brief LogbookWidget::repopulateModel (public slot) - cleanly re-populates the model to cater for a change
 * to the database connection (for example, when a backup is created or restored)
 */
void LogbookWidget::repopulateModel()
{
    // unset the current model and delete it to avoid leak
    view->setModel(nullptr);
    delete displayModel;
    // create a new model and populate it
    displayModel = new QSqlTableModel(this);
    setupModelAndView(Settings::read(Settings::Main::LogbookView).toInt());
    connectSignalsAndSlots();
}

void LogbookWidget::on_viewsComboBox_currentIndexChanged(int index)
{
    setupModelAndView(index);
}

void LogbookWidget::on_actionEdit_Flight_triggered()
{
    if(selectedEntries.length() == 1){
        NewFlightDialog nff(selectedEntries.first(), this);
        ui->stackedWidget->addWidget(&nff);
        ui->stackedWidget->setCurrentWidget(&nff);
        nff.setWindowFlag(Qt::Widget);
        nff.exec();
        displayModel->select();
    } else if (selectedEntries.isEmpty()) {
        WARN(tr("<br>No flight selected.<br>"));
    } else {
        WARN(tr("<br>More than one flight selected."
                               "<br><br>Editing multiple entries is not yet supported."));
    }
}

void LogbookWidget::on_actionEdit_Sim_triggered()
{
    if (selectedEntries.length() == 1) {
        NewSimDialog nsd((selectedEntries.first() * -1), this); // simulator entries have inverse row ID's in the model
        ui->stackedWidget->addWidget(&nsd);
        ui->stackedWidget->setCurrentWidget(&nsd);
        nsd.setWindowFlag(Qt::Widget);
        nsd.exec();
        displayModel->select();
    } else if (selectedEntries.isEmpty()) {
        WARN(tr("<br>No flight selected.<br>"));
    } else {
        WARN(tr("<br>More than one flight selected."
                               "<br><br>Editing multiple entries is not yet supported."));
    }
}

