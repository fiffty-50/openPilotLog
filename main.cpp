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
#include "src/opl.h"
#include "src/functions/log.h"
#include "src/gui/dialogues/firstrundialog.h"
#include "src/classes/runguard.h"
#include "src/classes/settings.h"
#include "src/classes/settings.h"
#include "src/classes/style.h"
#include "src/functions/log.h"
#include "src/classes/paths.h"
#include <QApplication>
#include <QProcess>
#include <QSettings>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>
#include <QTranslator>



/*!
 * \brief firstRun - is run if the application is run for the first time and launches
 * the FirstRunDialog which guides the user through the initial set-up process.
 */
bool firstRun()
{
    if(FirstRunDialog().exec() == QDialog::Rejected){
        LOG << "Initial setup incomplete or unsuccessful.";
        return false;
    }

    Settings::setSetupCompleted(true);
    LOG << "Initial Setup Completed successfully";
    QMessageBox mb;
    mb.setText("Initial set-up has been completed successfully.<br><br>Please re-start the application.");
    mb.exec();
    return true;
}


/*!
 * \brief init - Sets up the logging facilities, loads the user settings and sets
 * up the application style before the MainWindow is instantiated
 */
bool init()
{
    // Check if another instance of the application is already running, we don't want
    // different processes writing to the same database
    RunGuard guard(QStringLiteral("opl_single_key"));
    if ( !guard.tryToRun() ){
        LOG << "Another Instance of openPilotLog is already running. Exiting.";
        return false;
    }

    LOG << "Setting up / verifying Application Directories...";
    if(OPL::Paths::setup()) {
        LOG << "Application Directories... verified";
    } else {
        return false;
        LOG << "Unable to create directories.";
    }

    LOG << "Setting up logging facilities...";
    if(OPL::Log::init(true)) {
        LOG << "Logging enabled.";
    } else {
        LOG << "Unable to initalise logging.";
    }

    LOG << "Reading Settings...";
    Settings::init();
    LOG << "Setting up application style...";
    OPL::Style::setup();
    // Translations to be done at a later stage
    //LOG << "Installing translator...";
    //ATranslator::installTranslator(OPL::Translations::English);

    // Check for First Run and launch Setup Wizard
    if(!Settings::getSetupCompleted())
        return firstRun();

    return true;
}

int main(int argc, char *argv[])
{
    QApplication openPilotLog(argc, argv);
    QCoreApplication::setOrganizationName(ORGNAME);
    QCoreApplication::setOrganizationDomain(ORGDOMAIN);
    QCoreApplication::setApplicationName(APPNAME);

    // Set Up the Application
    if(!init())
        return 1;

    // Create Main Window and set Window Icon acc. to Platform
    MainWindow w;
#ifdef __linux__
    w.setWindowIcon(QIcon(OPL::Assets::ICON_APPICON_LINUX));
#elif defined(_WIN32) || defined(_WIN64)
    w.setWindowIcon(QIcon(OPL::Assets::ICON_APPICON_WIN));
#elif __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator
    #elif TARGET_OS_IPHONE
        // iOS device
    #elif TARGET_OS_MAC
    w.setWindowIcon(QIcon(OPL::Assets::ICON_APPICON_IOS));
    #else
    #   error "Unknown Apple platform"
    #endif
#endif

    //w.showMaximized();
    w.show();
    return openPilotLog.exec();
}
