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
#include <QToolBar>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "src/functions/alog.h"
#include "src/database/adatabase.h"
#include "src/classes/astyle.h"
#include "src/gui/dialogues/firstrundialog.h"
#include "src/classes/aentry.h"


// Quick and dirty Debug area
#include "src/gui/dialogues/newflightdialog.h"
#include "src/functions/adatetime.h"
void MainWindow::doDebugStuff()
{
    LOG << AStandardPaths::directory(AStandardPaths::Database).filesystemAbsolutePath();
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // connect to the Database
    QFileInfo database_file(AStandardPaths::directory(AStandardPaths::Database).
                                         absoluteFilePath(QStringLiteral("logbook.db")));
    bool db_invalid = false;
    if (!database_file.exists()) {
        WARN(tr("Error: Database file not found."));
        db_invalid = true;
    }
    if (database_file.size() == 0) { // To Do: Check for database errors instead of just checking for empty
        WARN(tr("Database file invalid."));
        db_invalid = true;
    }

    if (db_invalid)
        onDatabaseInvalid();


    if(!aDB->connect()){
        WARN(tr("Error establishing database connection."));
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

    backupWidget = new BackupWidget(this);
    ui->stackedWidget->addWidget(backupWidget);
    settingsWidget = new SettingsWidget(this);
    ui->stackedWidget->addWidget(settingsWidget);
    debugWidget = new DebugWidget(this);
    ui->stackedWidget->addWidget(debugWidget);

    connectWidgets();

    // Startup Screen (Home Screen)
    ui->stackedWidget->setCurrentWidget(homeWidget);

    // Create and set up the toolbar
    auto *toolBar = new QToolBar(this);

    ui->actionHome->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_HOME));
    toolBar->addAction(ui->actionHome);
    ui->actionNewFlight->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_NEW_FLIGHT));
    toolBar->addAction(ui->actionNewFlight);
    ui->actionLogbook->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_LOGBOOK));
    toolBar->addAction(ui->actionLogbook);
    ui->actionAircraft->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_AIRCRAFT));
    toolBar->addAction(ui->actionAircraft);
    ui->actionPilots->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_PILOT));
    toolBar->addAction(ui->actionPilots);
    ui->actionBackup->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_BACKUP));
    toolBar->addAction(ui->actionBackup);
    ui->actionSettings->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_SETTINGS));
    toolBar->addAction(ui->actionSettings);
    ui->actionQuit->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_QUIT));
    toolBar->addAction(ui->actionQuit);
    toolBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    toolBar->setMovable(false);
    addToolBar(Qt::ToolBarArea::LeftToolBarArea, toolBar);

    // check database version (Debug)
    if (aDB->dbRevision() < aDB->getMinimumDatabaseRevision()) {
        QString message = tr("Your database is out of date."
                             "Minimum required revision: %1<br>"
                             "You have revision: %2<br>")
                             .arg(aDB->getMinimumDatabaseRevision(), aDB->dbRevision());
        WARN(message);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
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
    QObject::connect(aDB,            &ADatabase::dataBaseUpdated,
                     homeWidget,     &HomeWidget::refresh);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     homeWidget,     &HomeWidget::refresh);

    QObject::connect(aDB,            &ADatabase::dataBaseUpdated,
                     logbookWidget,  &LogbookWidget::refresh);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     logbookWidget,  &LogbookWidget::onLogbookWidget_viewSelectionChanged);

    QObject::connect(aDB,            &ADatabase::dataBaseUpdated,
                     aircraftWidget, &AircraftWidget::onAircraftWidget_dataBaseUpdated);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     aircraftWidget, &AircraftWidget::onAircraftWidget_settingChanged);

    QObject::connect(aDB, &ADatabase::dataBaseUpdated,
                     pilotsWidget,   &PilotsWidget::onPilotsWidget_databaseUpdated);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     pilotsWidget,   &PilotsWidget::onPilotsWidget_settingChanged);

    QObject::connect(aDB,             &ADatabase::connectionReset,
                     logbookWidget,   &LogbookWidget::repopulateModel);
    QObject::connect(aDB,             &ADatabase::connectionReset,
                     pilotsWidget,    &PilotsWidget::repopulateModel);
    QObject::connect(aDB,             &ADatabase::connectionReset,
                     aircraftWidget,  &AircraftWidget::repopulateModel);
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
            if(!aDB->restoreBackup(db_path)) {
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

void MainWindow::on_actionBackup_triggered()
{
    ui->stackedWidget->setCurrentWidget(backupWidget);
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

// Debug

// not used at the moment

/*void MainWindow::on_actionNewAircraft_triggered()
{
    NewTailDialog nt(QString(), this);
    nt.exec();
}

void MainWindow::on_actionNewPilot_triggered()
{
    NewPilotDialog np(this);
    np.exec();
}*/
