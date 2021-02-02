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
        DEB << "Standard paths not valid.";
        return 1;
    }

    ASettings::setup();

    AStyle::setup();

    if (!aDB->connect()) {
        DEB << "Error establishing database connection";
        return 2;
    }

    if (!ASettings::read(ASettings::Setup::SetupComplete).toBool()) {
        if(FirstRunDialog().exec() == QDialog::Rejected){
            DEB << "First run not accepted. Exiting.";
            return 3;
        }
        ASettings::write(ASettings::Setup::SetupComplete, true);
        DEB << "Wrote setup_commplete?";
    }

    ARunGuard guard(QStringLiteral("opl_single_key"));
    if ( !guard.tryToRun() ){
        DEB << "Another Instance of openPilotLog is already running. Exiting.";
        return 0;
    }


    MainWindow w;
    //w.showMaximized();
    w.show();
    return openPilotLog.exec();
}
