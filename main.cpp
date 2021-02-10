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

    if (!aDB->connect()) {
        LOG << "Error establishing database connection\n";
        return 2;
    }

    if (!ASettings::read(ASettings::Main::SetupComplete).toBool()) {
        if(FirstRunDialog().exec() == QDialog::Rejected){
            LOG << "Initial setup incomplete or unsuccessfull. Exiting.\n";
            return 3;
        }
        ASettings::write(ASettings::Main::SetupComplete, true);
        DEB << "Wrote setup_commplete";
    }

    ARunGuard guard(QStringLiteral("opl_single_key"));
    if ( !guard.tryToRun() ){
        LOG << "Another Instance of openPilotLog is already running. Exiting.\n";
        return 0;
    }

    MainWindow w;
    w.setWindowIcon(QIcon(Opl::Assets::APP_ICON_FILLED));
    //w.showMaximized();
    w.show();
    return openPilotLog.exec();
}
