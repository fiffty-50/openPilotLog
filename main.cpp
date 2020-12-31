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
#include "src/gui/dialogues/firstrundialog.h"
#include "src/classes/arunguard.h"
#include "src/database/adatabase.h"
#include "src/classes/asettings.h"
#include "src/astandardpaths.h"
#include "src/classes/asettings.h"
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

    AStandardPaths::setup();
    AStandardPaths::scan_paths();
    if(!AStandardPaths::validate_paths()){
        DEB << "Standard paths not valid.";
        return 1;
    }

    ASettings::setup();

    FirstRunDialog().exec();
    aDB()->connect();
    if (!ASettings::read(QStringLiteral("setup/setup_complete")).toBool()) {
        FirstRunDialog dialog;
        if(dialog.exec() == QDialog::Accepted) {
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        } else {
            DEB "First run not accepted.";
            return 0;
        }

    }

    //Theming
    switch (ASettings::read(QStringLiteral("main/theme")).toInt()) {
    case 1:{
        DEB << "main :: Loading light theme";
        QFile file(":light.qss");
        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream stream(&file);
        openPilotLog.setStyleSheet(stream.readAll());
        break;
    }
    case 2:{
        DEB << "Loading dark theme";
        QFile file(":dark.qss");
        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream stream(&file);
        openPilotLog.setStyleSheet(stream.readAll());
        break;
    }
    default:
        break;
    }

    //sqlite does not deal well with multiple connections, ensure only one instance is running
    ARunGuard guard(QStringLiteral("opl_single_key"));
        if ( !guard.tryToRun() ){
            DEB << "Another Instance is already running. Exiting.";
            return 0;
        }

    MainWindow w;
    //w.showMaximized();
    w.show();
    return openPilotLog.exec();
}
