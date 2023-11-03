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
#include <QToolBar>

#include "src/gui/widgets/homewidget.h"
#include "src/gui/widgets/settingswidget.h"
#include "src/gui/widgets/logbookwidget.h"
#include "src/gui/widgets/tableeditwidget.h"
#include "src/gui/widgets/tailswidget.h"
#include "src/gui/widgets/airportwidget.h"
#include "src/gui/widgets/airportwidget.h"
#include "src/gui/widgets/pilotswidget.h"
#include "src/gui/widgets/debugwidget.h"
#include "src/classes/style.h"

enum Style {Light, Dark};
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*!
 * \brief The MainWindow contains a QStackedWidget and a QToolBar as the main User Interface.
 * \details The Tool bar contains shortcuts to the different widgets, which are on selection set active on the stacked main widget.
 * For a detailed description of what each widget does, please refer to the documentation for each widget. This is only a short synopsis:
 *
 * ## HomeWidget
 *
 * The home widget displays the total amount of hours for all logged flights, seperated into different categories. It also enables keeping track
 * of currencies and license expiries
 *
 * ## New Flight
 *
 * Opens a NewFlightDialog which can be used to submit a new flight to the database.
 *
 * ## Logboook
 *
 * Shows a view of the logbook table in a QTableView and enables editing the entries by spawning a child NewFlightDialog with the details for a selected flight.
 *
 * ## Aircraft
 *
 * Shows a view of the tails table in a QTableView and enables editing the entries by spawning a child NewTailDialog with the details for a selected tail.
 *
 * ## Pilots
 *
 * Shows a view of the pilots table in a QTableView and enables editing the entries by spawning a child NewPilotDialog with the details for a selected pilot.
 *
 * ## Airports
 *
 * Enables viewing and editing the airports database
 *
 * ## Settings
 *
 * Enables changing application settings
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onStyleChanged(SettingsWidget::SettingSignal signal){
        if (signal == SettingsWidget::MainWindow)
            setActionIcons(OPL::Style::getStyleType());
    }

private slots:

    void on_actionHome_triggered();

    void on_actionNewFlight_triggered();

    void on_actionLogbook_triggered();

    void on_actionAircraft_triggered();

    void on_actionPilots_triggered();

    void on_actionAirports_triggered();

    void on_actionSettings_triggered();

    void on_actionQuit_triggered();

    void on_actionDebug_triggered();

    void on_actionNewSim_triggered();

private:
    Ui::MainWindow *ui;

    HomeWidget* homeWidget;

    LogbookWidget* logbookWidget;
    
    TableEditWidget* tailsWidget;

    TableEditWidget* pilotsWidget;

    TableEditWidget* airportWidget;

    SettingsWidget* settingsWidget;

    DebugWidget* debugWidget;

    bool airportDbIsDirty = false;

    void init();
    void setupToolbar();
    void initialiseWidgets();
    void connectDatabase();
    void setActionIcons(OPL::Style::StyleType style = OPL::Style::StyleType::Light);

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

    /*!
     * \brief resizeEvent Resize the Toolbar's icon size to match the window height
     */
    void resizeEvent(QResizeEvent *event) override
    {
        //DEB << "SIZE:" << this->size();
        int icon_size;
        if (this->height() < 760)
            icon_size = (this->height() / 16);
        else
            icon_size = (this->height() / 14);

        auto tool_bar = this->findChild<QToolBar*>();
        tool_bar->setIconSize(QSize(icon_size, icon_size));
        event->accept();
    }

    //void closeEvent(QCloseEvent *event) override; //TODO check and prompt for creation of backup?
};
#endif // MAINWINDOW_H
