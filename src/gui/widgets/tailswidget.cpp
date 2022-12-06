/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
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
#include "tailswidget.h"
#include "ui_aircraftwidget.h"
#include "src/opl.h"
#include "src/classes/settings.h"
#include "src/database/database.h"
#include "src/database/row.h"
#include "src/gui/dialogues/newtaildialog.h"


TailsWidget::TailsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AircraftWidget)
{
    ui->setupUi(this);

    ui->tableView->setMinimumWidth(this->width()/2);
    ui->stackedWidget->setMinimumWidth(this->width()/2);

    setupModelAndView();
}

TailsWidget::~TailsWidget()
{
    delete ui;
}

void TailsWidget::setupModelAndView()
{
    model = new QSqlTableModel(this);
    model->setTable(QStringLiteral("viewTails"));
    model->select();

    view = ui->tableView;
    view->setModel(model);

    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection); // For now, editing multiple entries is not supported.
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->horizontalHeader()->setStretchLastSection(QHeaderView::Stretch);
    view->hideColumn(0);
    view->resizeColumnsToContents();
    view->verticalHeader()->hide();
    view->setAlternatingRowColors(true);

    sortColumn = Settings::read(Settings::UserData::TailSortColumn).toInt();
    view->setSortingEnabled(true);
    view->sortByColumn(sortColumn, Qt::DescendingOrder);

    view->show();
    selection = view->selectionModel();

    connectSignalsAndSlots();
}

void TailsWidget::connectSignalsAndSlots()
{
    QObject::connect(ui->tableView->selectionModel(),   &QItemSelectionModel::selectionChanged,
                     this,                              &TailsWidget::tableView_selectionChanged);
    QObject::connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked,
                     this,                              &TailsWidget::tableView_headerClicked);
}

/*
 * Slots
 */

void TailsWidget::onAircraftWidget_settingChanged(SettingsWidget::SettingSignal signal)
{
    if (signal != SettingsWidget::AircraftWidget)
        return;

    setupModelAndView();
}

void TailsWidget::onAircraftWidget_dataBaseUpdated()
{
    refreshView();
}

void TailsWidget::changeEvent(QEvent *event)
{
    if (event != nullptr)
        if(event->type() == QEvent::LanguageChange)
            ui->retranslateUi(this);
}

void TailsWidget::on_newAircraftButton_clicked()
{
    NewTailDialog nt(QString(), this);
    setUiEnabled(false);
    nt.exec();
    refreshView();
    setUiEnabled(true);
}

/*!
 * \brief Displays a dialog in which the Tail can be edited
 */
void TailsWidget::tableView_selectionChanged()
{
    if (this->findChild<NewTailDialog*>() != nullptr)
        delete this->findChild<NewTailDialog*>();

    selectedTails.clear();
    const auto selected_rows = selection->selectedRows();
    for (const auto& row : selected_rows) {
        selectedTails << row.data().toInt();
        DEB << "Selected Tails(s) with ID: " << selectedTails;
    }

    if(selectedTails.length() == 1) {
        NewTailDialog nt(selectedTails.first(), this);
        nt.setWindowFlag(Qt::Widget);
        ui->stackedWidget->addWidget(&nt);
        ui->stackedWidget->setCurrentWidget(&nt);

        setUiEnabled(false);
        nt.exec();
        refreshView();
        setUiEnabled(true);
    }
}

/*!
 * \brief Acts as a filter on the display model
 */
void TailsWidget::on_aircraftSearchLineEdit_textChanged(const QString &arg1)
{
    if(ui->aircraftSearchComboBox->currentIndex() == 0){
        ui->aircraftSearchLineEdit->setText(arg1.toUpper());
    }
    model->setFilter(ui->aircraftSearchComboBox->currentText()
                     + QLatin1String(" LIKE \"%")
                     + arg1 + QLatin1String("%\""));
}

void TailsWidget::tableView_headerClicked(int column)
{
    sortColumn = column;
    Settings::write(Settings::UserData::TailSortColumn, column);
}

void TailsWidget::on_deleteAircraftButton_clicked()
{
    if (selectedTails.length() == 0) {
        INFO(tr("No Aircraft selected."));

    } else if (selectedTails.length() > 1) {
        WARN(tr("Deleting multiple entries is currently not supported"));
        /// [F] to do: for (const auto& row_id : selectedPilots) { do batchDelete }
        /// I am not sure if enabling this functionality for this widget is a good idea.
        /// On the one hand, deleting many entries could be useful in a scenario where
        /// for example, the user has changed airlines and does not want to have his 'old'
        /// colleagues polluting his logbook anymore.
        /// On the other hand we could run into issues with foreign key constraints on the
        /// flights table (see on_delete_unsuccessful) below.
        /// I think batch-editing should be implemented at some point, but batch-deleting should not.

    } else if (selectedTails.length() == 1) {
        auto entry = DB->getTailEntry(selectedTails.first());
        QMessageBox message_box(this);
        message_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        message_box.setDefaultButton(QMessageBox::No);
        message_box.setIcon(QMessageBox::Question);
        message_box.setWindowTitle(tr("Delete Aircraft"));
        message_box.setText(tr("You are deleting the following aircraft:<br><br><b><tt>"
                               "%1 - (%2)</b></tt><br><br>Are you sure?"
                               ).arg(entry.getData().value(OPL::Db::TAILS_REGISTRATION).toString(),
                                     getAircraftTypeString(entry)));

        if (message_box.exec() == QMessageBox::Yes) {
            if(!DB->remove(entry))
                onDeleteUnsuccessful();
        }
    }
    refreshView();
    ui->stackedWidget->setCurrentIndex(0);
    ui->aircraftSearchLineEdit->setText(QString());
}

/*!
 * \brief Informs the user that deleting a database entry has been unsuccessful
 *
 * \details Normally, when one of these entries can not be deleted, it is because of
 * a [foreign key constraint](https://sqlite.org/foreignkeys.html), meaning that a flight
 * is associated with the aircraft that was supposed to be deleted.
 *
 * This function is used to inform the user and give hints on how to solve the problem.
 */
void TailsWidget::onDeleteUnsuccessful()
{
    QList<int> foreign_key_constraints = DB->getForeignKeyConstraints(selectedTails.first(),
                                                                       OPL::DbTable::Tails);
    QList<OPL::FlightEntry> constrained_flights;
    for (const auto &row_id : qAsConst(foreign_key_constraints)) {
        constrained_flights.append(DB->getFlightEntry(row_id));
    }

    QMessageBox message_box(this);
    if (constrained_flights.isEmpty()) {
        message_box.setText(tr("<br>Unable to delete.<br><br>The following error has ocurred: %1"
                               ).arg(DB->lastError.text()));
        message_box.exec();
        return;
    } else {
        QString constrained_flights_string;
        for (int i=0; i<constrained_flights.length(); i++) {
            constrained_flights_string.append(getFlightSummary(constrained_flights[i]) + QLatin1String("&nbsp;&nbsp;&nbsp;&nbsp;<br>"));
            if (i>10) {
                constrained_flights_string.append(QLatin1String("<br>[...]<br>"));
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

void TailsWidget::setUiEnabled(bool enabled)
{
    ui->newAircraftButton->setEnabled(enabled);
    ui->deleteAircraftButton->setEnabled(enabled);
    ui->tableView->setEnabled(enabled);
    ui->aircraftSearchComboBox->setEnabled(enabled);
    ui->aircraftSearchComboBox->setEnabled(enabled);
}

void TailsWidget::repopulateModel()
{
    // unset the current model and delete it to avoid leak
    view->setModel(nullptr);
    delete model;
    // create a new model and populate it
    model = new QSqlTableModel(this);
    setupModelAndView();
    connectSignalsAndSlots();
}

const QString TailsWidget::getAircraftTypeString(const OPL::Row &row) const
{
    QString type_string;
    if (!row.getData().value(OPL::Db::TAILS_MAKE).toString().isEmpty())
        type_string.append(row.getData().value(OPL::Db::TAILS_MAKE).toString() + QLatin1Char(' '));
    if (!row.getData().value(OPL::Db::TAILS_MODEL).toString().isEmpty())
        type_string.append(row.getData().value(OPL::Db::TAILS_MODEL).toString());
    if (!row.getData().value(OPL::Db::TAILS_VARIANT).toString().isEmpty())
        type_string.append(QLatin1Char('-') + row.getData().value(OPL::Db::TAILS_VARIANT).toString());

    return type_string;
}

const QString TailsWidget::getFlightSummary(const OPL::FlightEntry &flight) const
{
    if(!flight.isValid())
        return QString();

    auto tableData = flight.getData();
    QString flight_summary;
    auto space = QLatin1Char(' ');
    flight_summary.append(tableData.value(OPL::Db::FLIGHTS_DOFT).toString() + space);
    flight_summary.append(tableData.value(OPL::Db::FLIGHTS_DEPT).toString() + space);
    flight_summary.append(OPL::Time::toString(tableData.value(OPL::Db::FLIGHTS_TOFB).toInt())
                          + space);
    flight_summary.append(OPL::Time::toString(tableData.value(OPL::Db::FLIGHTS_TONB).toInt())
                          + space);
    flight_summary.append(tableData.value(OPL::Db::FLIGHTS_DEST).toString());

    return flight_summary;
}
