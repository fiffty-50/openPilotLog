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
#include "aircraftwidget.h"
#include "ui_aircraftwidget.h"
#include "src/gui/dialogues/newtaildialog.h"
#include "src/classes/asettings.h"
#include "src/database/adatabase.h"
#include "src/classes/atailentry.h"
#include "src/classes/aflightentry.h"
#include "src/testing/adebug.h"

AircraftWidget::AircraftWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AircraftWidget)
{
    DEB << "New AircraftWidet";
    ui->setupUi(this);
    ui->stackedWidget->addWidget(this->findChild<QWidget*>("welcomePageTails"));
    ui->stackedWidget->setCurrentWidget(this->findChild<QWidget*>("welcomePageTails"));

    setupModelAndView();
}

AircraftWidget::~AircraftWidget()
{
    DEB << "Deleting NewAircraftWidget";
    delete ui;
}

/*
 * Functions
 */

void AircraftWidget::setupModelAndView()
{
    sortColumn = ASettings::read(ASettings::UserData::AcftSortColumn).toInt();

    model = new QSqlTableModel(this);
    model->setTable("viewTails");
    model->select();

    view = ui->tableView;
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
    view->sortByColumn(sortColumn, Qt::DescendingOrder);

    view->show();
    selection = view->selectionModel();

    QObject::connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this, &AircraftWidget::tableView_selectionChanged);
    QObject::connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked,
                     this, &AircraftWidget::tableView_headerClicked);
}

/*
 * Slots
 */
void AircraftWidget::on_deleteButton_clicked()
{
    if (selectedTails.length() == 0) {
        QMessageBox message_box(this);
        message_box.setText(tr("No Aircraft selected."));
        message_box.exec();

    } else if (selectedTails.length() > 1) {
        QMessageBox message_box(this);
        message_box.setText(tr("Deleting multiple entries is currently not supported"));
        message_box.exec();
        /// [F] to do: for (const auto& row_id : selectedPilots) { do batchDelete }
        /// I am not sure if enabling this functionality for this widget is a good idea.
        /// On the one hand, deleting many entries could be useful in a scenario where
        /// for example, the user has changed airlines and does not want to have his 'old'
        /// colleagues polluting his logbook anymore.
        /// On the other hand we could run into issues with foreign key constraints on the
        /// flights table (see on_delete_unsuccessful) below.
        /// I think batch-editing should be implemented at some point, but batch-deleting should not.

    } else if (selectedTails.length() == 1) {
        auto entry = aDB->getTailEntry(selectedTails.first());
        QMessageBox message_box(this);
        message_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        message_box.setDefaultButton(QMessageBox::No);
        message_box.setIcon(QMessageBox::Question);
        message_box.setWindowTitle(tr("Delete Aircraft"));
        message_box.setText(tr("You are deleting the following aircraft:<br><br><b><tt>"
                               "%1 - (%2)</b></tt><br><br>Are you sure?"
                               ).arg(entry.registration(),
                                     entry.type()));

        if (message_box.exec() == QMessageBox::Yes) {
            if(!aDB->remove(entry))
                onDeleteUnsuccessful();
        }
    }

    model->select();
}

void AircraftWidget::onDeleteUnsuccessful()
{
    QList<int> foreign_key_constraints = aDB->getForeignKeyConstraints(selectedTails.first(),
                                                                       ADatabaseTarget::tails);
    QList<AFlightEntry> constrained_flights;
    for (const auto &row_id : qAsConst(foreign_key_constraints)) {
        constrained_flights.append(aDB->getFlightEntry(row_id));
    }

    QMessageBox message_box(this);
    if (constrained_flights.isEmpty()) {
        message_box.setText(tr("<br>Unable to delete.<br><br>The following error has ocurred: %1"
                               ).arg(aDB->lastError.text()));
        message_box.exec();
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
        message_box.setText(tr("Unable to delete.<br><br>"
                               "This is most likely the case because a flight exists with the aircraft "
                               "you are trying to delete.<br><br>"
                               "%1 flight(s) with this aircraft have been found:<br><br><br><b><tt>"
                               "%2"
                               "</b></tt><br><br>You have to change or remove the conflicting flight(s) "
                               "before removing this aircraft from the database.<br><br>"
                               ).arg(QString::number(constrained_flights.length()), constrained_flights_string));
        message_box.setIcon(QMessageBox::Critical);
        message_box.exec();
    }
}

void AircraftWidget::on_newAircraftButton_clicked()
{
    NewTailDialog nt(QString(), this);
    connect(&nt, SIGNAL(accepted()), this, SLOT(acft_editing_finished()));
    connect(&nt, SIGNAL(rejected()), this, SLOT(acft_editing_finished()));
    nt.exec();
}

void AircraftWidget::on_aircraftSearchLineEdit_textChanged(const QString &arg1)
{
    if(ui->aircraftSearchComboBox->currentIndex() == 0){
        ui->aircraftSearchLineEdit->setText(arg1.toUpper());
    }
    model->setFilter(ui->aircraftSearchComboBox->currentText()
                     + QStringLiteral(" LIKE \"%")
                     + arg1 + QStringLiteral("%\""));
}

void AircraftWidget::onDisplayModel_dataBaseUpdated()
{
    //refresh view to reflect changes the user has made via a dialog.
    model->select();
}

void AircraftWidget::tableView_selectionChanged()
{
    if (this->findChild<NewTailDialog*>() != nullptr) {
        delete this->findChild<NewTailDialog*>();
        /// [F] if the user changes the selection without making any changes,
        /// if(selectedTails.length() == 1) spawns a new dialog without the
        /// old one being deleted, since neither accept() nor reject() was emitted.
        /// Is this a reasonable way of avoiding pollution? Creating the widgets on the
        /// stack does not seem to solve the problem since the Dialog does not get destroyed
        /// until either accept() or reject() is emitted so I went for this solution.
    }
    selectedTails.clear();

    for (const auto& row : selection->selectedRows()) {
        selectedTails << row.data().toInt();
        DEB << "Selected Tails(s) with ID: " << selectedTails;
    }
    if(selectedTails.length() == 1) {
        auto* nt = new NewTailDialog(selectedTails.first(), this);
        QObject::connect(nt,   &QDialog::accepted,
                         this, &AircraftWidget::acft_editing_finished);
        QObject::connect(nt,   &QDialog::rejected,
                         this, &AircraftWidget::acft_editing_finished);
        ui->stackedWidget->addWidget(nt);
        ui->stackedWidget->setCurrentWidget(nt);
        nt->setWindowFlag(Qt::Widget);
        nt->setAttribute(Qt::WA_DeleteOnClose);
        nt->exec();
    }
}

void AircraftWidget::tableView_headerClicked(int column)
{
    sortColumn = column;
    ASettings::write(ASettings::UserData::AcftSortColumn, column);
}

void AircraftWidget::acft_editing_finished()
{
    model->select();
}
