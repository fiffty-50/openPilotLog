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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QSqlTableModel>
#include <QTableView>
#include <chrono>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QKeyEvent>

#include "src/gui/widgets/homewidget.h"
#include "src/gui/widgets/settingswidget.h"
#include "src/gui/widgets/logbookwidget.h"
#include "src/gui/widgets/aircraftwidget.h"
#include "src/gui/widgets/backupwidget.h"
#include "src/gui/widgets/pilotswidget.h"
#include "src/gui/widgets/debugwidget.h"
#include "src/gui/dialogues/newtaildialog.h"
#include "src/gui/dialogues/newpilotdialog.h"
#include "src/gui/dialogues/newflightdialog.h"
#include "src/classes/arunguard.h"
#include "src/classes/acompletiondata.h"
#include "src/testing/atimer.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_actionHome_triggered();

    void on_actionNewFlight_triggered();

    void on_actionLogbook_triggered();

    void on_actionAircraft_triggered();

    //void on_actionNewAircraft_triggered();

    void on_actionPilots_triggered();

    //void on_actionNewPilot_triggered();

    void on_actionBackup_triggered();

    void on_actionSettings_triggered();

    void on_actionQuit_triggered();

    void on_actionDebug_triggered();

private:
    Ui::MainWindow *ui;

    HomeWidget* homeWidget;

    LogbookWidget* logbookWidget;

    AircraftWidget* aircraftWidget;

    PilotsWidget* pilotsWidget;

    BackupWidget* backupWidget;

    SettingsWidget* settingsWidget;

    DebugWidget* debugWidget;

    // Completion Data for QCompleters and Mapping
    ACompletionData completionData;

    void nope();

    void connectWidgets();

    // Prompts the user to fix a broken database or import a backup
    void onDatabaseInvalid();

    //
    void doDebugStuff();

protected:
    /*!
     * \brief Shows the debug widget by pressing <ctrl + t>
     * <Shift+Enter for a quick and dirty debug>
     */
    void keyPressEvent(QKeyEvent* keyEvent) override
    {
        if(keyEvent->type() == QKeyEvent::KeyPress) {
            if(keyEvent->matches(QKeySequence::AddTab)) {
                on_actionDebug_triggered();
            } else if (keyEvent->matches(QKeySequence::InsertLineSeparator)) {
                doDebugStuff();
            }
        }
    }
};
#endif // MAINWINDOW_H
