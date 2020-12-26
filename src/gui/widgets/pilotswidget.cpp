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
#include "pilotswidget.h"
#include "ui_pilotswidget.h"
#include "src/testing/adebug.h"

using namespace experimental;
PilotsWidget::PilotsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PilotsWidget)
{
    ui->setupUi(this);

    setupModelAndView();
}

PilotsWidget::~PilotsWidget()
{
    delete ui;
}

void PilotsWidget::setupModelAndView()
{
    sortColumn = ASettings::read("userdata/pilSortColumn").toInt();

    model = new QSqlTableModel(this);
    model->setTable("viewPilots");
    model->setFilter("ID > 1");//to not allow editing of self, shall be done via settings
    model->select();

    view = ui->pilotsTableView;
    view->setModel(model);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->hideColumn(0);
    view->setColumnWidth(1, 180);
    view->setColumnWidth(2, 180);
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->setSortingEnabled(true);
    view->sortByColumn(sortColumn, Qt::AscendingOrder);

    view->show();
    selectionModel = view->selectionModel();

    QObject::connect(ui->pilotsTableView->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &PilotsWidget::tableView_selectionChanged);
    QObject::connect(ui->pilotsTableView->horizontalHeader(), &QHeaderView::sectionClicked,
                     this, &PilotsWidget::tableView_headerClicked);
}

void PilotsWidget::on_pilotSearchLineEdit_textChanged(const QString &arg1)
{
    model->setFilter("\"" + ui->pilotsSearchComboBox->currentText() + "\" LIKE \"%" + arg1 + "%\" AND ID > 1");
}

void PilotsWidget::onDisplayModel_dataBaseUpdated()
{
    //refresh view to reflect changes the user has made via a dialog.
    model->select();
}

void PilotsWidget::tableView_selectionChanged()//const QItemSelection &index, const QItemSelection &
{
    if (this->findChild<NewPilotDialog*>() != nullptr) {
        DEB("Selection changed. Deleting orphaned dialog.");
        delete this->findChild<NewPilotDialog*>();
        /// [F] if the user changes the selection without making any changes,
        /// if(selectedPilots.length() == 1) spawns a new dialog without the
        /// old one being deleted, since neither accept() nor reject() was emitted.
        /// Is this a reasonable way of avoiding pollution? Creating the widgets on the
        /// stack does not seem to solve the problem since the Dialog does not get deleted
        /// until either accept() or reject() is emitted so I went for this solution.
    }
    auto *selection = ui->pilotsTableView->selectionModel();
    selectedPilots.clear();

    for (const auto& row : selection->selectedRows()) {
        selectedPilots.append(row.data().toInt());
        DEB("Selected Tails(s) with ID: " << selectedPilots);
    }
    if(selectedPilots.length() == 1) {

        NewPilotDialog* np = new NewPilotDialog(selectedPilots.first(), this);
        QObject::connect(np, &QDialog::accepted,
                         this, &PilotsWidget::pilot_editing_finished);
        QObject::connect(np, &QDialog::rejected,
                         this, &PilotsWidget::pilot_editing_finished);
        np->setWindowFlag(Qt::Widget);
        np->setAttribute(Qt::WA_DeleteOnClose);
        ui->stackedWidget->addWidget(np);
        ui->stackedWidget->setCurrentWidget(np);
        np->exec();
    }
}

void PilotsWidget::tableView_headerClicked(int column)
{
    sortColumn = column;
    ASettings::write("userdata/pilSortColumn", column);
}

void PilotsWidget::on_newPilotButton_clicked()
{
    NewPilotDialog* np = new NewPilotDialog(this);
    QObject::connect(np,   &QDialog::accepted,
                     this, &PilotsWidget::pilot_editing_finished);
    QObject::connect(np,   &QDialog::rejected,
                     this, &PilotsWidget::pilot_editing_finished);
    np->setAttribute(Qt::WA_DeleteOnClose);
    np->exec();
}

void PilotsWidget::on_deletePilotButton_clicked()
{
    if (selectedPilots.length() == 0) {
        auto mb = QMessageBox(this);
        mb.setText("No Pilot selected.");
        mb.exec();

    } else if (selectedPilots.length() > 1) {
        auto mb = QMessageBox(this);
        mb.setText("Deleting multiple entries is currently not supported");
        mb.exec();
        /// [F] to do: for (const auto& row_id : selectedPilots) { do batchDelete }
        /// I am not sure if enabling this functionality for this widget is a good idea.
        /// On the one hand, deleting many entries could be useful in a scenario where
        /// for example, the user has changed airlines and does not want to have his 'old'
        /// colleagues polluting his logbook anymore.
        /// On the other hand we could run into issues with foreign key constraints on the
        /// flights table (see on_delete_unsuccessful) below.
        /// I think batch-editing should be implemented at some point, but batch-deleting should not.

    } else if (selectedPilots.length() == 1) {
        auto entry = aDB()->getPilotEntry(selectedPilots.first());
        auto message_box = QMessageBox(this);
        QString message = "You are deleting the following pilot:<br><br><b><tt>";
        message.append(entry.name());
        message.append(QStringLiteral("</b></tt><br><br>Are you sure?"));
        message_box.setText(message);
        message_box.exec();
        if(!aDB()->remove(entry))
            onDeleteUnsuccessful();
        }
    model->select();
}

void PilotsWidget::onDeleteUnsuccessful()
{
    /// [F]: To do: Some logic to display a warning if too many entries exists, so that
    /// the messagebox doesn't grow too tall.
    QList<int> foreign_key_constraints = aDB()->getForeignKeyConstraints(selectedPilots.first(), ADatabaseTarget::pilots);
    QList<AFlightEntry> constrained_flights;
    for (const auto &row_id : foreign_key_constraints) {
        constrained_flights.append(aDB()->getFlightEntry(row_id));
    }

    QString message = "<br>Unable to delete.<br><br>";
    if(!constrained_flights.isEmpty()){
        message.append(QStringLiteral("This is most likely the case because a flight exists with the Pilot "
                   "you are trying to delete as PIC.<br>"
                   "The following flight(s) with this pilot have been found:<br><br><br><b><tt>"));
        for (auto &flight : constrained_flights) {
            message.append(flight.summary() + QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;<br>"));
        }
    }
    message.append(QStringLiteral("</b></tt><br><br>You have to change or remove the conflicting flight(s) "
                                  "before removing this pilot from the database.<br><br>"));
    QMessageBox message_box(this);
    message_box.setText(message);
    message_box.setIcon(QMessageBox::Critical);
    message_box.exec();
}

void PilotsWidget::pilot_editing_finished()
{
    model->select();
}
