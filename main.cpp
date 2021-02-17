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
#include "src/gui/dialogues/firstrundialog.h"
#include "src/classes/arunguard.h"
#include "src/database/adatabase.h"
#include "src/classes/asettings.h"
#include "src/classes/astandardpaths.h"
#include "src/classes/asettings.h"
#include "src/classes/astyle.h"
#include "src/oplconstants.h"
#include "src/functions/alog.h"
#include <QApplication>
#include <QProcess>
#include <QSettings>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>

#define APPNAME QStringLiteral("openPilotLog")
#define ORGNAME QStringLiteral("opl")
#define ORGDOMAIN QStringLiteral("https://github.com/fiffty-50/openpilotlog")

int main(int argc, char *argv[])
{
    QApplication openPilotLog(argc, argv);
    QCoreApplication::setOrganizationName(ORGNAME);
    QCoreApplication::setOrganizationDomain(ORGDOMAIN);
    QCoreApplication::setApplicationName(APPNAME);

    if(!AStandardPaths::setup()){
        LOG << "Unable to create directories.\n";
        return 1;
    }

    ASettings::setup();

    AStyle::setup();

    if (ASettings::read(ASettings::Main::SetupComplete).toBool()) {
        QFileInfo database_file(AStandardPaths::directory(AStandardPaths::Database).
                                     absoluteFilePath(QStringLiteral("logbook.db")));
        if (!database_file.exists()) {
            LOG << "Error: Database file not found\n";
            return 2;
        }
    } else {
        if(FirstRunDialog().exec() == QDialog::Rejected){
            LOG << "Initial setup incomplete or unsuccessfull. Exiting.\n";
            return 3;
        }
        ASettings::write(ASettings::Main::SetupComplete, true);
        DEB << "Wrote setup_commplete";
    }

    if (!aDB->connect()) {
        LOG << "Error establishing database connection\n";
        return 4;
    }

    ARunGuard guard(QStringLiteral("opl_single_key"));
    if ( !guard.tryToRun() ){
        LOG << "Another Instance of openPilotLog is already running. Exiting.\n";
        return 0;
    }

    MainWindow w;

#ifdef __linux__
    w.setWindowIcon(QIcon(Opl::Assets::ICON_APPICON_LINUX));
#elif defined(_WIN32) || defined(_WIN64)
    w.setWindowIcon(QIcon(Opl::Assets::ICON_APPICON_WIN));
#elif __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator
    #elif TARGET_OS_IPHONE
        // iOS device
    #elif TARGET_OS_MAC
    w.setWindowIcon(QIcon(Opl::Assets::ICON_APPICON_IOS));
    #else
    #   error "Unknown Apple platform"
    #endif
#endif

    //w.showMaximized();
    w.show();
    return openPilotLog.exec();
}
