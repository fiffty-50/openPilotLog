#include "logbookwidget.h"
#include "src/classes/settings.h"
#include "src/database/database.h"
#include "src/gui/dialogues/flightlogentryeditdialog.h"
#include "src/gui/dialogues/simentryeditdialog.h"
#include "src/gui/views/logbookview.h"
#include "src/gui/views/viewdefault.h"
#include "src/gui/views/vieweasa.h"
#include "src/opl.h"
#include <memory>
#include <qabstractitemmodel.h>
#include <qassert.h>
#include <qnamespace.h>

LogbookWidget::LogbookWidget(QWidget *parent) : QWidget{parent} { init(); }

void LogbookWidget::init()
{
    DEB << "Setting up UI...";
    ui = new Ui::LogbookWidgetUi();
    ui->setupUi(this);

    m_logbookView = Settings::getLogbookView();
    if (!setViewHelper()) WARN(tr("Unable to set view helper."));
    m_flightEditDialog = new OPL::FlightLogEntryEditDialog(this);
    m_simEditDialog    = new SimEntryEditDialog(this);
    ui->stackedWidget->addWidget(m_flightEditDialog);
    ui->stackedWidget->addWidget(m_simEditDialog);

    setupModelAndView();
    setupSignals();
}

void LogbookWidget::setupModelAndView()
{
    DEB << "Setting up view...";
    QString view_name = m_viewHelper->databaseName();
    LOG << "Loading Logbook View: " << view_name;

    ui->model = new QSqlTableModel(this, DB->database());
    ui->model->setTable(view_name);
    ui->model->select();
    ui->view->setModel(ui->model);

    m_viewHelper->setupView(ui->model, ui->view);
}

void LogbookWidget::viewSelectionChanged(SettingsWidget::SettingSignal widget)
{
    if (widget == SettingsWidget::SettingSignal::LogbookWidget) {
        m_logbookView = Settings::getLogbookView();
        if (!setViewHelper())
            WARN(tr("Unable to set view helper for %1")
                     .arg(OPL::GLOBALS->getLogbookViewName(m_logbookView)));
        setupModelAndView();
    }
}

void LogbookWidget::setupSignals()
{
    // refresh the view when the database is updated
    QObject::connect(DB, &OPL::Database::dataBaseUpdated, this, [this]() { ui->model->select(); });
    // sort the view by column
    QObject::connect(
        ui->view->horizontalHeader(), &QHeaderView::sectionClicked, this,
        [this](int sort_column) { ui->view->sortByColumn(sort_column, Qt::AscendingOrder); });
    // Edit an entry when selected
    QObject::connect(ui->view, &QTableView::clicked, this,
                     [this](const QModelIndex &index) { openEntryEdit(index); });
    // Hide the edit widget when editing is finished
    QObject::connect(m_flightEditDialog, &QDialog::finished, this,
                     [this]() { ui->stackedWidget->hide(); });
    QObject::connect(m_simEditDialog, &QDialog::finished, this,
                     [this]() { ui->stackedWidget->hide(); });

    // Add a new Flight Entry
    QObject::connect(ui->newFlightButton, &QPushButton::clicked, this, [this]() {
        auto dialog = new OPL::FlightLogEntryEditDialog(this);
        dialog->exec();
    });
    // Add a new Simulator Entry
    QObject::connect(ui->newSimButton, &QPushButton::clicked, this, [this]() {
        auto dialog = new SimEntryEditDialog(this);
        dialog->exec();
    });
    // Delete a selected entry
    QObject::connect(ui->deleteButton, &QPushButton::clicked, this, [this]() {
        auto index = ui->view->selectionModel()->currentIndex();
        if (!index.isValid()) {
            WARN(tr("No Entry Selected"));
            return;
        }
        deleteEntryRequested(index);
    });
}

void LogbookWidget::deleteEntryRequested(const QModelIndex &index)
{
    DEB << "Delete requested for index: " << index;
    ui->view->selectionModel()->reset();
    ui->stackedWidget->hide();

    // get user confirmation
    const auto answer =
        QMessageBox::question(this, tr("Confirm Deletion"), confirmDeleteString(index),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (answer == QMessageBox::Yes) {
        OPL::LogEvent event_type = m_viewHelper->eventType(index);
        EntryEditDialog *dialog;
        switch (event_type) {
        case OPL::LogEvent::Flight:
            dialog = m_flightEditDialog;
            break;
        case OPL::LogEvent::Sim:
            dialog = m_simEditDialog;
            break;
        }

        if (!dialog->deleteEntry(m_viewHelper->eventId(index))) WARN(errorOnDeleteString(index));
    }
}

QString LogbookWidget::confirmDeleteString(const QModelIndex &index)
{
    return m_viewHelper->getSummaryString(index);
}

QString LogbookWidget::errorOnDeleteString(const QModelIndex &index)
{
    Event event = {m_viewHelper->eventType(index), m_viewHelper->eventId(index)};
    switch (event.type) {
    case OPL::LogEvent::Flight: {
        auto constraints = DB->getForeignKeyConstraints(event.rowId, OPL::DbTable::Flights);
        if (constraints.isEmpty()) {
            return DB->lastErrorText();
        }
        TODO << "Show constraints";
        return {};
    }
    case OPL::LogEvent::Sim:
        return DB->lastErrorText();
    }
    return {};
}

void LogbookWidget::openEntryEdit(const QModelIndex &index)
{
    DEB << "Opening entry edit for index: " << index;
    Event event             = {m_viewHelper->eventType(index), m_viewHelper->eventId(index)};
    EntryEditDialog *dialog = nullptr;

    switch (event.type) {
    case OPL::LogEvent::Flight:
        dialog = m_flightEditDialog;
        break;
    case OPL::LogEvent::Sim:
        dialog = m_simEditDialog;
        break;
    }

    if (event.rowId > 0)
        dialog->loadEntry(event.rowId);
    else
        dialog->reset();

    ui->stackedWidget->setCurrentWidget(dialog);
    ui->stackedWidget->show();
    dialog->open();
}

bool LogbookWidget::setViewHelper()
{
    DEB << "Setting view helper...";
    DEB << "logbook view: " << OPL::GLOBALS->getLogbookViewName(m_logbookView);

    switch (m_logbookView) {
    case OPL::LogbookView::Default:
        m_viewHelper = std::make_unique<ViewDefault>();
        return true;
    case OPL::LogbookView::DefaultWithSim:
        DEB << "with sim";
        m_viewHelper = std::make_unique<ViewDefaultWithSim>();
        return true;
    case OPL::LogbookView::Easa:
        m_viewHelper = std::make_unique<ViewEasa>();
        return true;
    case OPL::LogbookView::EasaWithSim:
        DEB << "with sim";
        m_viewHelper = std::make_unique<ViewEasaWithSim>();
        return true;
    case OPL::LogbookView::SimulatorOnly:
        return false;
    }
    return true;
}
