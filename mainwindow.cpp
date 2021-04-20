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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "src/functions/alog.h"
#include "src/database/adatabase.h"
#include "src/classes/astyle.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create a spacer for the toolbar to separate left and right parts
    auto *spacer = new QWidget();
    spacer->setMinimumWidth(1);
    spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    // Set up Toolbar
    ui->actionHome->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_HOME));
    ui->actionNewFlight->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_NEW_FLIGHT));
    ui->actionLogbook->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_LOGBOOK));
    ui->actionAircraft->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_AIRCRAFT));
    ui->actionPilots->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_PILOT));
    ui->toolBar->insertWidget(ui->actionSettings, spacer); // spacer goes here
    ui->actionBackup->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_BACKUP));
    ui->actionSettings->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_SETTINGS));
    ui->actionQuit->setIcon(QIcon(Opl::Assets::ICON_TOOLBAR_QUIT));
    ui->toolBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    const auto buttons = ui->toolBar->findChildren<QWidget *>();
    ui->toolBar->setIconSize(QSize(64, 64));
    for (const auto &button : buttons) {
        button->setMinimumWidth(128);
    }

    // Construct Widgets
    homeWidget = new HomeWidget(this);
    ui->stackedWidget->addWidget(homeWidget);
    logbookWidget = new LogbookWidget(this);
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


    // check database version (Debug)
    int db_ver = aDB->dbVersion();
    if (db_ver != DATABASE_REVISION) {
        DEB << "############## WARNING ##############";
        DEB << "Your database is out of date.";
        DEB << "Current Revision:\t" << DATABASE_REVISION;
        DEB << "You have revision:\t" << db_ver;
        DEB << "############## WARNING ##############";
        QMessageBox message_box(this); //error box
        message_box.setText(tr("Database revision out of date!"));
        message_box.exec();
    } else {
        DEB << "Your database is up to date with the latest revision:" << db_ver;
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

/*
 * Slots
 */

void MainWindow::on_actionHome_triggered()
{
    ui->stackedWidget->setCurrentWidget(homeWidget);
}

void MainWindow::on_actionNewFlight_triggered()
{
    NewFlightDialog nf(this);
    nf.exec();
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
