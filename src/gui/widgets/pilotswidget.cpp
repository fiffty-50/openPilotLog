/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#include "src/opl.h"
#include "src/functions/alog.h"
#include "src/database/adatabase.h"
#include "src/classes/apilotentry.h"

PilotsWidget::PilotsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PilotsWidget)
{
    ui->setupUi(this);

    ui->tableView->setMinimumWidth(this->width()/2);
    ui->stackedWidget->setMinimumWidth(this->width()/2);

    setupModelAndView();
}

PilotsWidget::~PilotsWidget()
{
    delete ui;
}

void PilotsWidget::setupModelAndView()
{
    model = new QSqlTableModel(this);
    model->setTable(QStringLiteral("viewPilots"));
    model->setFilter(QStringLiteral("ID > 1")); // Don't show self
    model->select();

    view = ui->tableView;
    view->setModel(model);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->hideColumn(0);
    view->resizeColumnsToContents();
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);
    view->setSortingEnabled(true);
    sortColumn = ASettings::read(ASettings::UserData::PilotSortColumn).toInt();
    view->sortByColumn(sortColumn, Qt::AscendingOrder);

    view->show();
    selectionModel = view->selectionModel();

    connectSignalsAndSlots();
}

void PilotsWidget::connectSignalsAndSlots()
{
    QObject::connect(ui->tableView->selectionModel(),   &QItemSelectionModel::selectionChanged,
                     this,                              &PilotsWidget::tableView_selectionChanged);
    QObject::connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked,
                     this,                              &PilotsWidget::tableView_headerClicked);
}

void PilotsWidget::changeEvent(QEvent *event)
{
    if (event != nullptr)
        if(event->type() == QEvent::LanguageChange)
            ui->retranslateUi(this);
}

void PilotsWidget::onPilotsWidget_settingChanged(SettingsWidget::SettingSignal signal)
{
    if (signal == SettingsWidget::PilotsWidget)
        setupModelAndView();
}

void PilotsWidget::onPilotsWidget_databaseUpdated()
{
    refreshView();
}

void PilotsWidget::onNewPilotDialog_editingFinished()
{
    refreshView();
}

void PilotsWidget::on_pilotSearchLineEdit_textChanged(const QString &arg1)
{
    model->setFilter(QLatin1Char('\"') + ui->pilotsSearchComboBox->currentText()
                     + QLatin1String("\" LIKE '%") + arg1
                     + QLatin1String("%' AND ID > 1"));
}

void PilotsWidget::tableView_selectionChanged()
{
    if (this->findChild<NewPilotDialog*>() != nullptr) {
        delete this->findChild<NewPilotDialog*>();
    }

    auto *selection = ui->tableView->selectionModel();
    selectedPilots.clear();

    for (const auto& row : selection->selectedRows()) {
        selectedPilots.append(row.data().toInt());
        DEB << "Selected Tails(s) with ID: " << selectedPilots;
    }
    if(selectedPilots.length() == 1) {

        NewPilotDialog* np = new NewPilotDialog(selectedPilots.first(), this);
        QObject::connect(np,   &QDialog::accepted,
                         this, &PilotsWidget::onNewPilotDialog_editingFinished);
        QObject::connect(np,   &QDialog::rejected,
                         this, &PilotsWidget::onNewPilotDialog_editingFinished);
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
    ASettings::write(ASettings::UserData::PilotSortColumn, column);
}

void PilotsWidget::on_newPilotButton_clicked()
{
    NewPilotDialog* np = new NewPilotDialog(this);
    QObject::connect(np,   &QDialog::accepted,
                     this, &PilotsWidget::onNewPilotDialog_editingFinished);
    QObject::connect(np,   &QDialog::rejected,
                     this, &PilotsWidget::onNewPilotDialog_editingFinished);
    np->setAttribute(Qt::WA_DeleteOnClose);
    np->exec();
}

void PilotsWidget::on_deletePilotButton_clicked()
{
    if (selectedPilots.length() == 0) {
        INFO(tr("No Pilot selected."));
    } else if (selectedPilots.length() > 1) {
        WARN(tr("Deleting multiple entries is currently not supported"));
        /// [F] to do: for (const auto& row_id : selectedPilots) { do batchDelete }
        /// I am not sure if enabling this functionality for this widget is a good idea.
        /// On the one hand, deleting many entries could be useful in a scenario where
        /// for example, the user has changed airlines and does not want to have his 'old'
        /// colleagues polluting his logbook anymore.
        /// On the other hand we could run into issues with foreign key constraints on the
        /// flights table (see on_delete_unsuccessful) below.
        /// I think batch-editing should be implemented at some point, but batch-deleting should not.

    } else if (selectedPilots.length() == 1) {
        auto entry = aDB->getPilotEntry(selectedPilots.first());
        QMessageBox confirm(this);
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.setIcon(QMessageBox::Question);
        confirm.setWindowTitle(tr("Delete Pilot"));

        confirm.setText(tr("You are deleting the following pilot:<br><br><b><tt>"
                               "%1</b></tt><br><br>Are you sure?").arg(entry.name()));
        if (confirm.exec() == QMessageBox::Yes) {
            if(!aDB->remove(entry))
                onDeleteUnsuccessful();
        }
    }
    refreshView();
}

/*!
 * \brief Informs the user that deleting a database entry has been unsuccessful
 *
 * \details Normally, when one of these entries can not be deleted, it is because of
 * a [foreign key constraint](https://sqlite.org/foreignkeys.html), meaning that a flight
 * is associated with the Pilot that was supposed to be deleted as Pilot-in-command.
 *
 * This function is used to inform the user and give hints on how to solve the problem.
 */
void PilotsWidget::onDeleteUnsuccessful()
{
    const QList<int> foreign_key_constraints = aDB->getForeignKeyConstraints(selectedPilots.first(),
                                                                       ADatabaseTables::pilots);
    QList<AFlightEntry> constrained_flights;
    for (const auto &row_id : foreign_key_constraints) {
        constrained_flights.append(aDB->getFlightEntry(row_id));
    }

    if (constrained_flights.isEmpty()) {
        WARN(tr("<br>Unable to delete.<br><br>The following error has ocurred:<br>%1"
                               ).arg(aDB->lastError.text()));
        return;
    } else {
        QString constrained_flights_string;
        for (int i=0; i<constrained_flights.length(); i++) {
            constrained_flights_string.append(constrained_flights[i].summary() + QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;<br>"));
            if (i>10) {
                constrained_flights_string.append("<br>[...]<br>");
                break;
            }
        }
        WARN(tr("Unable to delete.<br><br>"
                               "This is most likely the case because a flight exists with the Pilot "
                               "you are trying to delete as PIC.<br><br>"
                               "%1 flight(s) with this pilot have been found:<br><br><br><b><tt>"
                               "%2"
                               "</b></tt><br><br>You have to change or remove the conflicting flight(s) "
                               "before removing this pilot from the database.<br><br>"
                               ).arg(QString::number(constrained_flights.length()),
                                     constrained_flights_string));
    }
}

void PilotsWidget::repopulateModel()
{
    // unset the current model and delete it to avoid leak
    view->setModel(nullptr);
    delete model;
    // create a new model and populate it
    model = new QSqlTableModel(this);
    setupModelAndView();
    connectSignalsAndSlots();
}
