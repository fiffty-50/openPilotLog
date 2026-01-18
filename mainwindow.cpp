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
#include "mainwindow.h"
#include "src/classes/settings.h"
#include "src/classes/style.h"
#include "src/database/database.h"
#include "src/database/databasecache.h"
#include "src/database/databasesetup.h"
#include "src/gui/dialogues/firstrundialog.h"
#include "src/gui/dialogues/flightentryeditdialog.h"
#include "src/gui/dialogues/pilotentryeditdialog.h"
#include "src/gui/dialogues/simentryeditdialog.h"
#include "src/gui/dialogues/tailentryeditdialog.h"
#include "src/gui/widgets/databaseeditwidget.h"
#include "src/gui/widgets/logbooktableeditwidget.h"
#include "src/gui/widgets/pilottableeditwidget.h"
#include "src/gui/widgets/tailtableeditwidget.h"
#include "ui_mainwindow.h"
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::init()
{
    connectDatabase();
    initialiseWidgets();
    setupToolbar();
    connectWidgets();
    setActionIcons(OPL::Style::getStyleType());
    ui->stackedWidget->setCurrentWidget(homeWidget);
}

void MainWindow::setupToolbar()
{
    // Create and set up the toolbar
    auto *toolBar = new QToolBar(this);

    toolBar->addAction(ui->actionHome);
    toolBar->addAction(ui->actionNewFlight);
    toolBar->addAction(ui->actionNewSim);
    toolBar->addAction(ui->actionNewTail);
    toolBar->addAction(ui->actionNewPilot);
    toolBar->addAction(ui->actionLogbook);
    toolBar->addAction(ui->actionDatabase);
    toolBar->addAction(ui->actionSettings);
    toolBar->addAction(ui->actionQuit);
    toolBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    toolBar->setMovable(false);
    addToolBar(Qt::ToolBarArea::LeftToolBarArea, toolBar);
}

void MainWindow::initialiseWidgets()
{
    // Construct Widgets
    homeWidget = new HomeWidget(this);
    ui->stackedWidget->addWidget(homeWidget);

    logbookWidget = new LogbookTableEditWidget(this);
    logbookWidget->init();
    ui->stackedWidget->addWidget(logbookWidget);

    tailsWidget = new TailTableEditWidget(this);
    tailsWidget->init();
    ui->stackedWidget->addWidget(tailsWidget);

    pilotsWidget = new PilotTableEditWidget(this);
    pilotsWidget->init();
    ui->stackedWidget->addWidget(pilotsWidget);

    databaseWidget = new DatabaseEditWidget(this);
    ui->stackedWidget->addWidget(databaseWidget);

    settingsWidget = new SettingsWidget(this);
    ui->stackedWidget->addWidget(settingsWidget);

    debugWidget = new DebugWidget(this);
    ui->stackedWidget->addWidget(debugWidget);
}

void MainWindow::connectDatabase()
{
    // connect to the Database
    if (OPL::Paths::databaseFileInfo().size() == 0) onDatabaseInvalid();

    if (!DB->connect()) {
        WARN(
            tr("Error establishing database connection. The following error has ocurred:<br><br>%1")
                .arg(DB->lastError.text()));
    }

    // Load Cache
    DBCache->init();
}

void MainWindow::setActionIcons(OPL::Style::StyleType style)
{
    switch (style) {
    case OPL::Style::StyleType::Light:
        LOG << "Setting Light Icon theme";
        ui->actionHome->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_HOME));
        ui->actionNewFlight->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_NEW_FLIGHT));
        ui->actionNewSim->setIcon(
            QIcon(OPL::Assets::ICON_TOOLBAR_NEW_FLIGHT)); // TODO seperate icon
        ui->actionLogbook->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_LOGBOOK));
        ui->actionNewTail->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_AIRCRAFT));
        ui->actionNewPilot->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_PILOT));
        ui->actionDatabase->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_BACKUP));
        ui->actionSettings->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_SETTINGS));
        ui->actionQuit->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_QUIT));
        break;
    case OPL::Style::StyleType::Dark:
        LOG << "Setting Dark Icon theme";
        ui->actionHome->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_HOME_DARK));
        ui->actionNewFlight->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_NEW_FLIGHT_DARK));
        ui->actionNewSim->setIcon(
            QIcon(OPL::Assets::ICON_TOOLBAR_NEW_FLIGHT_DARK)); // pending separate icon
        ui->actionLogbook->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_LOGBOOK_DARK));
        ui->actionNewTail->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_AIRCRAFT_DARK));
        ui->actionNewPilot->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_PILOT_DARK));
        ui->actionDatabase->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_BACKUP_DARK));
        ui->actionSettings->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_SETTINGS_DARK));
        ui->actionQuit->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_QUIT_DARK));
        break;
    }
}

void MainWindow::nope()
{
    QMessageBox message_box(this); // error box
    message_box.setText(tr("This feature is not yet available!"));
    message_box.exec();
}

/*!
 * \brief Connect the widgets to signals that are emitted when an update of the widegts' contents,
 * is required, either because the database has changed (model and view need to be refreshed) or
 * because a setting that affects the widgets layout has changed.
 */
void MainWindow::connectWidgets()
{
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged, logbookWidget,
                     &LogbookTableEditWidget::viewSelectionChanged);
    QObject::connect(this, &MainWindow::addFlightEntryRequested, logbookWidget,
                     &LogbookTableEditWidget::addEntryRequested);
    QObject::connect(this, &MainWindow::addSimulatorEntryRequested, logbookWidget,
                     &LogbookTableEditWidget::addSimulatorEntryRequested);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged, this,
                     &MainWindow::onStyleChanged);
}

void MainWindow::onDatabaseInvalid()
{
    QMessageBox db_error(this);
    db_error.addButton(tr("Restore Backup"), QMessageBox::ButtonRole::AcceptRole);
    db_error.addButton(tr("Create New Database"), QMessageBox::ButtonRole::RejectRole);
    db_error.addButton(tr("Abort"), QMessageBox::ButtonRole::DestructiveRole);
    db_error.setIcon(QMessageBox::Warning);
    db_error.setWindowTitle(tr("No valid database found"));
    db_error.setText(tr("No valid database has been found.<br>"
                        "Would you like to create a new database or import a backup?<br><br>"));

    int ret = db_error.exec();
    if (ret == QMessageBox::DestructiveRole) {
        DEB << "No valid database found. Exiting.";
        on_actionQuit_triggered();
    }
    else if (ret == QMessageBox::ButtonRole::AcceptRole) {
        DEB << "Yes(Import Backup)";
        QString db_path = QDir::toNativeSeparators((QFileDialog::getOpenFileName(
            this, tr("Select Database"), OPL::Paths::directory(OPL::Paths::Backup).canonicalPath(),
            tr("Database file (*.db)"))));
        if (!db_path.isEmpty()) {
            if (!DB->restoreBackup(db_path)) {
                WARN(tr("Unable to restore backup file:<br><br>%1").arg(db_path));
                on_actionQuit_triggered();
            }
        }
    }
    else if (ret == QMessageBox::ButtonRole::RejectRole) {
        DEB << "No(Create New)";
        if (FirstRunDialog().exec() == QDialog::Rejected) {
            LOG << "Initial setup incomplete or unsuccessfull.";
            on_actionQuit_triggered();
        }
        Settings::setSetupCompleted(true);
        LOG << "Initial Setup Completed successfully";
    }
}

/*
 * Slots
 */

void MainWindow::on_actionHome_triggered() { ui->stackedWidget->setCurrentWidget(homeWidget); }

void MainWindow::on_actionNewFlight_triggered()
{
    auto newFlightDialog = new FlightEntryEditDialog(0, this);
    newFlightDialog->exec();
}

void MainWindow::on_actionNewSim_triggered()
{
    auto newSimDialog = new SimEntryEditDialog(0, this);
    newSimDialog->exec();
}

void MainWindow::on_actionLogbook_triggered()
{
    ui->stackedWidget->setCurrentWidget(logbookWidget);
}

void MainWindow::on_actionNewTail_triggered()
{
    ui->stackedWidget->setCurrentWidget(databaseWidget);
    databaseWidget->addEntry(DatabaseEditWidget::Tails);
}

void MainWindow::on_actionNewPilot_triggered()
{
    ui->stackedWidget->setCurrentWidget(databaseWidget);
    databaseWidget->addEntry(DatabaseEditWidget::Pilots);
}

void MainWindow::on_actionDatabase_triggered()
{
    ui->stackedWidget->setCurrentWidget(databaseWidget);
}

void MainWindow::on_actionSettings_triggered()
{
    ui->stackedWidget->setCurrentWidget(settingsWidget);
}

void MainWindow::on_actionQuit_triggered() { QApplication::quit(); }

void MainWindow::on_actionDebug_triggered() { ui->stackedWidget->setCurrentWidget(debugWidget); }

// WIP area - pressing SHIFT + ENTER executes this function
// this is to provide easy and quick access to a currently worked on functionality
#include <QStyleFactory>
void MainWindow::debug()
{

    // auto dbSetup = DatabaseSetup();
    // dbSetup.clearDatabase();
    // dbSetup.createTables();
    // dbSetup.importTemplateData(false);
    int id = 2;
    DEB << "Type String; " << OPL::AircraftEntry::getTypeString(2);
    auto entry = DB->getAircraftEntry(2);
    DEB << entry;
}
