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
#include <QToolBar>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "src/functions/alog.h"
#include "src/database/database.h"
#include "src/classes/astyle.h"
#include "src/gui/dialogues/firstrundialog.h"
#include "src/gui/dialogues/newflightdialog.h"
#include "src/gui/dialogues/newsimdialog.h"
// Quick and dirty Debug area
#include <QTimeZone>
void MainWindow::doDebugStuff()
{
    const auto list = QTimeZone::availableTimeZoneIds();
    DEB << list;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupToolbar();
    setActionIcons(AStyle::getStyleType());

    // connect to the Database
    QFileInfo database_file(AStandardPaths::directory(AStandardPaths::Database).
                                         absoluteFilePath(QStringLiteral("logbook.db")));

    if (database_file.size() == 0)
        onDatabaseInvalid();

    if(!DB->connect()){
        WARN(tr("Error establishing database connection. The following error has ocurred:<br><br>%1")
             .arg(DB->lastError.text()));
    }

    // retreive completion lists and maps
    completionData.init();

    // Construct Widgets
    homeWidget = new HomeWidget(this);
    ui->stackedWidget->addWidget(homeWidget);
    logbookWidget = new LogbookWidget(completionData, this);
    ui->stackedWidget->addWidget(logbookWidget);
    aircraftWidget = new AircraftWidget(this);
    ui->stackedWidget->addWidget(aircraftWidget);
    pilotsWidget = new PilotsWidget(this);
    ui->stackedWidget->addWidget(pilotsWidget);

    airportWidget = new AirportWidget(this);
    ui->stackedWidget->addWidget(airportWidget);
    settingsWidget = new SettingsWidget(this);
    ui->stackedWidget->addWidget(settingsWidget);
    debugWidget = new DebugWidget(this);
    ui->stackedWidget->addWidget(debugWidget);

    connectWidgets();

    // Startup Screen (Home Screen)
    ui->stackedWidget->setCurrentWidget(homeWidget);

    // check database version (Debug)
    //if (DB->dbRevision() < DB->getMinimumDatabaseRevision()) {
    //    QString message = tr("Your database is out of date."
    //                         "Minimum required revision: %1<br>"
    //                         "You have revision: %2<br>")
    //                         .arg(DB->getMinimumDatabaseRevision(), DB->dbRevision());
    //    WARN(message);
    //}
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupToolbar()
{
    // Create and set up the toolbar
    auto *toolBar = new QToolBar(this);

    toolBar->addAction(ui->actionHome);
    toolBar->addAction(ui->actionNewFlight);
    toolBar->addAction(ui->actionNewSim);
    toolBar->addAction(ui->actionLogbook);
    toolBar->addAction(ui->actionAircraft);
    toolBar->addAction(ui->actionPilots);
    toolBar->addAction(ui->actionAirports);
    toolBar->addAction(ui->actionSettings);
    toolBar->addAction(ui->actionQuit);
    toolBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    toolBar->setMovable(false);
    addToolBar(Qt::ToolBarArea::LeftToolBarArea, toolBar);
}

void MainWindow::setActionIcons(StyleType style)
{
    switch (style){
    case StyleType::Light:
        LOG << "Setting Light Icon theme";
        ui->actionHome->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_HOME));
        ui->actionNewFlight->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_NEW_FLIGHT));
        ui->actionNewSim->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_NEW_FLIGHT)); // pending seperate icon
        ui->actionLogbook->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_LOGBOOK));
        ui->actionAircraft->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_AIRCRAFT));
        ui->actionPilots->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_PILOT));
        ui->actionAirports->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_BACKUP));
        ui->actionSettings->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_SETTINGS));
        ui->actionQuit->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_QUIT));
        break;
    case StyleType::Dark:
        LOG << "Setting Dark Icon theme";
        ui->actionHome->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_HOME_DARK));
        ui->actionNewFlight->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_NEW_FLIGHT_DARK));
        ui->actionNewSim->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_NEW_FLIGHT_DARK)); // pending separate icon
        ui->actionLogbook->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_LOGBOOK_DARK));
        ui->actionAircraft->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_AIRCRAFT_DARK));
        ui->actionPilots->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_PILOT_DARK));
        ui->actionAirports->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_BACKUP_DARK));
        ui->actionSettings->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_SETTINGS_DARK));
        ui->actionQuit->setIcon(QIcon(OPL::Assets::ICON_TOOLBAR_QUIT_DARK));
        break;
    }
}

void MainWindow::nope()
{
    QMessageBox message_box(this); //error box
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
    QObject::connect(DB,             &OPL::Database::dataBaseUpdated,
                     homeWidget,     &HomeWidget::refresh);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     homeWidget,     &HomeWidget::refresh);

    QObject::connect(DB,             &OPL::Database::dataBaseUpdated,
                     logbookWidget,  &LogbookWidget::refresh);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     logbookWidget,  &LogbookWidget::onLogbookWidget_viewSelectionChanged);

    QObject::connect(DB,             &OPL::Database::dataBaseUpdated,
                     aircraftWidget, &AircraftWidget::onAircraftWidget_dataBaseUpdated);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     aircraftWidget, &AircraftWidget::onAircraftWidget_settingChanged);

    QObject::connect(DB,             &OPL::Database::dataBaseUpdated,
                     pilotsWidget,   &PilotsWidget::onPilotsWidget_databaseUpdated);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     pilotsWidget,   &PilotsWidget::onPilotsWidget_settingChanged);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     this,           &MainWindow::onStyleChanged);

    QObject::connect(DB,              &OPL::Database::connectionReset,
                     logbookWidget,   &LogbookWidget::repopulateModel);
    QObject::connect(DB,              &OPL::Database::connectionReset,
                     pilotsWidget,    &PilotsWidget::repopulateModel);
    QObject::connect(DB,              &OPL::Database::connectionReset,
                     aircraftWidget,  &AircraftWidget::repopulateModel);

    // Catch database updates to lazily update CompletionData
    QObject::connect(DB,              &OPL::Database::dataBaseUpdated,
                     this,            &MainWindow::onDatabaseUpdated);
}

void MainWindow::onDatabaseUpdated(const OPL::DbTable table)
{
    switch (table) {
    case OPL::DbTable::Pilots:
        DEB << "Pilots table updated...";
        completionData.updatePilots();
        break;
    case OPL::DbTable::Tails:
        DEB << "Tails table updated...";
        completionData.updateTails();
        break;
    case OPL::DbTable::Airports:
        DEB << "Airports table updated...";
        completionData.updateAirports();
        break;
    default:
        break;
    }
}

void MainWindow::onDatabaseInvalid()
{
    QMessageBox db_error(this);
    //db_error.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    db_error.addButton(tr("Restore Backup"), QMessageBox::ButtonRole::AcceptRole);
    db_error.addButton(tr("Create New Database"), QMessageBox::ButtonRole::RejectRole);
    db_error.addButton(tr("Abort"), QMessageBox::ButtonRole::DestructiveRole);
    db_error.setIcon(QMessageBox::Warning);
    db_error.setWindowTitle(tr("No valid database found"));
    db_error.setText(tr("No valid database has been found.<br>"
                       "Would you like to create a new database or import a backup?"));

    int ret = db_error.exec();
    if (ret == QMessageBox::DestructiveRole) {
        DEB << "No valid database found. Exiting.";
        on_actionQuit_triggered();
    } else if (ret == QMessageBox::ButtonRole::AcceptRole) {
        DEB << "Yes(Import Backup)";
        QString db_path = QFileDialog::getOpenFileName(this,
                                                       tr("Select Database"),
                                                       AStandardPaths::directory(AStandardPaths::Backup).canonicalPath(),
                                                       tr("Database file (*.db)"));
        if (!db_path.isEmpty()) {
            if(!DB->restoreBackup(db_path)) {
               WARN(tr("Unable to restore Backup file: %1").arg(db_path));
               on_actionQuit_triggered();
            } else {
                INFO(tr("Backup successfully restored."));
            }
        }
    } else if (ret == QMessageBox::ButtonRole::RejectRole){
        DEB << "No(Create New)";
        if(FirstRunDialog().exec() == QDialog::Rejected){
            LOG << "Initial setup incomplete or unsuccessfull.";
            on_actionQuit_triggered();
        }
        ASettings::write(ASettings::Main::SetupComplete, true);
        LOG << "Initial Setup Completed successfully";
    }
}

/*
 * Slots
 */

void MainWindow::on_actionHome_triggered()
{
    ui->stackedWidget->setCurrentWidget(homeWidget);
}

void MainWindow::on_actionNewFlight_triggered()
{
    completionData.update();
    auto* nf = new NewFlightDialog(completionData, this);
    nf->exec();
}

void MainWindow::on_actionLogbook_triggered()
{
    ui->stackedWidget->setCurrentWidget(logbookWidget);
}

void MainWindow::on_actionAircraft_triggered()
{
    ui->stackedWidget->setCurrentWidget(aircraftWidget);
}

void MainWindow::on_actionPilots_triggered()
{
    ui->stackedWidget->setCurrentWidget(pilotsWidget);
}

void MainWindow::on_actionAirports_triggered()
{
    ui->stackedWidget->setCurrentWidget(airportWidget);
}

void MainWindow::on_actionSettings_triggered()
{
    ui->stackedWidget->setCurrentWidget(settingsWidget);
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionDebug_triggered()
{
    ui->stackedWidget->setCurrentWidget(debugWidget);
}

void MainWindow::on_actionNewSim_triggered()
{
    auto nsd = NewSimDialog(this);
    nsd.exec();
}
