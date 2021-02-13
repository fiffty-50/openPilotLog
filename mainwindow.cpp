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
#include "src/testing/adebug.h"
#include "src/database/adatabase.h"
#include "src/classes/astyle.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up Toolbar
    ui->actionHome->setIcon(QIcon(Opl::Assets::ICON_HOME));
    ui->actionNewFlight->setIcon(QIcon(Opl::Assets::ICON_NEW_FLIGHT));
    ui->actionLogbook->setIcon(QIcon(":/icons/ionicon-icons/book-outline.png"));
    ui->actionAircraft->setIcon(QIcon(Opl::Assets::ICON_AIRCRAFT));
    ui->actionPilots->setIcon(QIcon(Opl::Assets::ICON_PILOT));
    ui->actionSettings->setIcon(QIcon(":/icons/ionicon-icons/settings-outline.png"));
    ui->actionQuit->setIcon(QIcon(":/icons/ionicon-icons/power-outline.png"));
    ui->toolBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    const auto buttons = ui->toolBar->findChildren<QWidget *>();
    ui->toolBar->setIconSize(QSize(64, 64));
    for (const auto &button : buttons) {
        button->setMinimumWidth(128);
    }

    // Add spacer in toolbar to separate left and right parts
    auto *spacer = new QWidget();
    spacer->setMinimumWidth(1);
    spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    ui->toolBar->insertWidget(ui->actionSettings, spacer);

    // Construct Widgets
    homeWidget = new HomeWidget(this);
    ui->stackedWidget->addWidget(homeWidget);
    pilotsWidget = new PilotsWidget(this);
    ui->stackedWidget->addWidget(pilotsWidget);
    logbookWidget = new LogbookWidget(this);
    ui->stackedWidget->addWidget(logbookWidget);
    settingsWidget = new SettingsWidget(this);
    ui->stackedWidget->addWidget(settingsWidget);
    aircraftWidget = new AircraftWidget(this);
    ui->stackedWidget->addWidget(aircraftWidget);
    debugWidget = new DebugWidget(this);
    ui->stackedWidget->addWidget(debugWidget);

    connectWidgets();

    // Startup Screen (Home Screen)
    ui->stackedWidget->setCurrentWidget(homeWidget);


    // check database version (Debug)
    int db_ver = checkDbVersion();
    if (db_ver != DATABASE_REVISION) {
        DEB << "############## WARNING ####################";
        DEB << "Your database is out of date.";
        DEB << "Current Revision:\t" << DATABASE_REVISION;
        DEB << "You have revision:\t" << db_ver;
        DEB << "############## WARNING ###################";
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

/*
 * Slots
 */

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionHome_triggered()
{
    ui->stackedWidget->setCurrentWidget(homeWidget);
}

void MainWindow::on_actionLogbook_triggered()
{
    ui->stackedWidget->setCurrentWidget(logbookWidget);
}

void MainWindow::on_actionDebug_triggered()
{
    ui->stackedWidget->setCurrentWidget(debugWidget);
}

void MainWindow::connectWidgets()
{
    QObject::connect(aDB, &ADatabase::dataBaseUpdated,
                     logbookWidget, &LogbookWidget::refresh);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     logbookWidget, &LogbookWidget::onLogbookWidget_viewSelectionChanged);

    QObject::connect(aDB, &ADatabase::dataBaseUpdated,
                     pilotsWidget, &PilotsWidget::onPilotsWidget_databaseUpdated);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     pilotsWidget, &PilotsWidget::onPilotsWidget_settingChanged);

    QObject::connect(aDB,            &ADatabase::dataBaseUpdated,
                     aircraftWidget, &AircraftWidget::onAircraftWidget_dataBaseUpdated);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     aircraftWidget, &AircraftWidget::onAircraftWidget_settingChanged);

    QObject::connect(aDB, &ADatabase::dataBaseUpdated,
                     homeWidget, &HomeWidget::refresh);
    QObject::connect(settingsWidget, &SettingsWidget::settingChanged,
                     homeWidget, &HomeWidget::refresh);
}

void MainWindow::on_actionSettings_triggered()
{
    ui->stackedWidget->setCurrentWidget(settingsWidget);
}

void MainWindow::on_actionPilots_triggered()
{
    ui->stackedWidget->setCurrentWidget(pilotsWidget);
}

void MainWindow::on_actionAircraft_triggered()
{
    ui->stackedWidget->setCurrentWidget(aircraftWidget);
}

void MainWindow::on_actionNewFlight_triggered()
{
    NewFlightDialog nf(this);
    nf.exec();
}

void MainWindow::on_actionNewAircraft_triggered()
{
    NewTailDialog nt(QString(), this);
    nt.exec();
}

void MainWindow::on_actionNewPilot_triggered()
{
    NewPilotDialog np(this);
    np.exec();
}

// Debug

int MainWindow::checkDbVersion()
{
    QSqlQuery query("SELECT COUNT(*) FROM changelog");
    query.next();
    return query.value(0).toInt();
}
