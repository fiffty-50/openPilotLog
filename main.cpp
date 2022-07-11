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
#include "src/opl.h"
#include "src/functions/alog.h"
#include "src/gui/dialogues/firstrundialog.h"
#include "src/classes/arunguard.h"
#include "src/database/database.h"
#include "src/classes/asettings.h"
#include "src/classes/astandardpaths.h"
#include "src/classes/asettings.h"
#include "src/classes/astyle.h"
#include "src/functions/alog.h"
#include "src/classes/atranslator.h"
#include <QApplication>
#include <QProcess>
#include <QSettings>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>
#include <QTranslator>

#define APPNAME QStringLiteral("openPilotLog")
#define ORGNAME QStringLiteral("opl")
#define ORGDOMAIN QStringLiteral("https://github.com/fiffty-50/openpilotlog")

/*!
 *  Helper functions that prepare and set up the application
 */
namespace Main {

void init()
{
    LOG << "Setting up / verifying Application Directories...";
    if(AStandardPaths::setup()) {
        LOG << "Application Directories... verified";
    } else {
        LOG << "Unable to create directories.";
    }
    LOG << "Setting up logging facilities...";
    if(ALog::init(true)) {
        LOG << "Logging enabled.";
    } else {
        LOG << "Unable to initalise logging.";
    }
    LOG << "Reading Settings...";
    ASettings::setup();
    LOG << "Setting up application style...";
    AStyle::setup();
    // Translations to be done at a later stage
    //LOG << "Installing translator...";
    //ATranslator::installTranslator(OPL::Translations::English);
}

bool firstRun()
{
    if(FirstRunDialog().exec() == QDialog::Rejected){
        LOG << "Initial setup incomplete or unsuccessfull.";
        return false;
    }
    ASettings::write(ASettings::Main::SetupComplete, true);
    LOG << "Initial Setup Completed successfully";
    return true;
}
} // namespace Main

int main(int argc, char *argv[])
{
    QApplication openPilotLog(argc, argv);
    QCoreApplication::setOrganizationName(ORGNAME);
    QCoreApplication::setOrganizationDomain(ORGDOMAIN);
    QCoreApplication::setApplicationName(APPNAME);

    // Check for another instance already running
    ARunGuard guard(QStringLiteral("opl_single_key"));
    if ( !guard.tryToRun() ){
        LOG << "Another Instance of openPilotLog is already running. Exiting.";
        return 0;
    }

    // Set Up the Application
    Main::init();

    // Check for First Run and launch Setup Wizard
    if (!ASettings::read(ASettings::Main::SetupComplete).toBool())
        if(!Main::firstRun())
            return 0;

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
