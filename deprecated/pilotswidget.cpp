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
#include "pilotswidget.h"
#include "ui_pilotswidget.h"
#include "src/gui/dialogues/newpilotdialog.h"
#include "src/opl.h"
#include "src/database/database.h"
#include "src/database/row.h"
#include "src/classes/time.h"
#include "src/classes/settings.h"

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
    sortColumn = Settings::getPilotSortColumn();
    view->sortByColumn(sortColumn, Qt::AscendingOrder);

    view->show();
    selectionModel = view->selectionModel();

    connectSignalsAndSlots();
}

void PilotsWidget::connectSignalsAndSlots()
{
    QObject::connect(ui->pilotSearchLineEdit,  &QLineEdit::textChanged,
                     this,                     &PilotsWidget::filterLineEdit_textChanged);
    QObject::connect(view->horizontalHeader(), &QHeaderView::sectionClicked,
                     this,                     &PilotsWidget::newSortColumnSelected);
    QObject::connect(view,					   &QTableView::clicked,
                     this, 			    	   &PilotsWidget::editRequested);
}

void PilotsWidget::setUiEnabled(bool enabled)
{
   ui->tableView->setEnabled(enabled);
   ui->newPilotButton->setEnabled(enabled);
   ui->deletePilotButton->setEnabled(enabled);
   ui->pilotSearchLineEdit->setEnabled(enabled);
   ui->pilotsSearchComboBox->setEnabled(enabled);
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

void PilotsWidget::filterLineEdit_textChanged(const QString &arg1)
{
    model->setFilter(QLatin1Char('\"') + ui->pilotsSearchComboBox->currentText()
                     + QLatin1String("\" LIKE '%") + arg1
                     + QLatin1String("%' AND ID > 1"));
}

void PilotsWidget::editRequested(const QModelIndex &index)
{
    int pilotID = model->index(index.row(), 0).data().toInt();

    NewPilotDialog np = NewPilotDialog(pilotID, this);
    np.setWindowFlag(Qt::Widget);
    ui->stackedWidget->addWidget(&np);
    ui->stackedWidget->setCurrentWidget(&np);
    np.exec();
    refreshView();
}

void PilotsWidget::newSortColumnSelected(int newSortColumn)
{
    Settings::setPilotSortColumn(newSortColumn);
}

void PilotsWidget::on_newPilotButton_clicked()
{
    NewPilotDialog np = NewPilotDialog(this);
    np.exec();
    refreshView();
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
        auto entry = DB->getPilotEntry(selectedPilots.first());
        QMessageBox confirm(this);
        confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        confirm.setDefaultButton(QMessageBox::No);
        confirm.setIcon(QMessageBox::Question);
        confirm.setWindowTitle(tr("Delete Pilot"));

        confirm.setText(tr("You are deleting the following pilot:<br><br><b><tt>"
                               "%1</b></tt><br><br>Are you sure?").arg(getPilotName(entry)));
        if (confirm.exec() == QMessageBox::Yes) {
            if(!DB->remove(entry))
                onDeleteUnsuccessful();
        }
    }
    refreshView();
    ui->stackedWidget->setCurrentIndex(0);
    ui->pilotSearchLineEdit->setText(QString());
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
    const QList<int> foreign_key_constraints = DB->getForeignKeyConstraints(selectedPilots.first(),
                                                                       OPL::DbTable::Pilots);
    QList<OPL::FlightEntry> constrained_flights;
    for (const auto &row_id : foreign_key_constraints) {
        constrained_flights.append(DB->getFlightEntry(row_id));
    }

    if (constrained_flights.isEmpty()) {
        WARN(tr("<br>Unable to delete.<br><br>The following error has ocurred:<br>%1"
                               ).arg(DB->lastError.text()));
        return;
    } else {
        QString constrained_flights_string;
        for (int i=0; i<constrained_flights.length(); i++) {
            constrained_flights_string.append(OPL::FlightEntry(constrained_flights[i]).getFlightSummary() + QStringLiteral("&nbsp;&nbsp;&nbsp;&nbsp;<br>"));
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

const QString PilotsWidget::getPilotName(const OPL::PilotEntry &pilot) const
{
    if (!pilot.isValid())
        return QString();

    return pilot.getLastName() + QLatin1String(", ") + pilot.getFirstName();
}
