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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "debug.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up Toolbar
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->toolBar->setIconSize(QSize(64, 64));
    auto buttons = ui->toolBar->findChildren<QWidget *>();
    for (const auto &button : buttons) {
        button->setMinimumWidth(128);
    }

    ui->actionHome->setIcon(QIcon(":/icons/ionicon-icons/home-outline.png"));
    ui->actionNewFlight->setIcon(QIcon(":/icons/ionicon-icons/airplane-outline.png"));
    ui->actionLogbook->setIcon(QIcon(":/icons/ionicon-icons/book-outline.png"));
    ui->actionAircraft->setIcon(QIcon(":/icons/ionicon-icons/airplane-outline.png"));
    ui->actionPilots->setIcon(QIcon(":/icons/ionicon-icons/settings-outline.png"));
    ui->actionDebug->setIcon(QIcon(":/icons/ionicon-icons/settings-outline.png"));
    ui->actionSettings->setIcon(QIcon(":/icons/ionicon-icons/settings-outline.png"));
    ui->actionQuit->setIcon(QIcon(":/icons/ionicon-icons/power-outline.png"));

    // Adds space between toolbar items
    auto *spacer = new QWidget();
    spacer->setMinimumWidth(10);
    spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    ui->toolBar->insertWidget(ui->actionDebug, spacer);


    DEB("Construction MainWindow Widgets\n");
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

    // Startup Screen (Home Screen)
    // ui->stackedWidget->setCurrentWidget(homeWidget);
    // Debup Widget for now
    debugWidget = new DebugWidget(this);
    ui->stackedWidget->addWidget(debugWidget);
    ui->stackedWidget->setCurrentWidget(debugWidget);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::nope()
{
    QMessageBox nope(this); //error box
    nope.setText("This feature is not yet available!");
    nope.exec();
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
    NewFlightDialog nf = NewFlightDialog(Db::createNew, this);
    nf.exec();

}

void MainWindow::on_actionNewAircraft_triggered()
{
    NewTailDialog nt = NewTailDialog(QString(), Db::createNew, this);
    nt.exec();
}

void MainWindow::on_actionNewPilot_triggered()
{
    NewPilotDialog np =NewPilotDialog(Db::createNew, this);
    np.exec();
}
