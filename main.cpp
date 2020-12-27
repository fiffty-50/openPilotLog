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
#include "src/experimental/adatabase.h"
#include "src/classes/asettings.h"
#include <QApplication>
#include <QProcess>
#include <QSettings>
#include <QFileInfo>
#include <QStandardPaths>

/// Utility struct for standard app paths. Struct should be created after QCoreApplication::set___Name.
struct StandardAppPaths{
    QMap<QStandardPaths::StandardLocation, QString> paths;
    StandardAppPaths()
        : paths({{QStandardPaths::AppConfigLocation, QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)},
                 {QStandardPaths::AppDataLocation, QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)},})
    {}
};

/// Ensure standard app paths exist, if not mkdir them.
static inline
void scan_paths(StandardAppPaths paths)
{
    for(auto& path : paths.paths.values()){
        auto dir = QDir(path);
        DEB("Scanning " << dir.path());
        if(!dir.exists())
            DEB("Creating " << dir.path());
            dir.mkdir(path);
    }
}

/// Validate standard app paths are valid in structure and contents.
static inline
bool validate_paths(StandardAppPaths paths)
{
    for(auto& path : paths.paths.values()){
        DEB("Validating " << path);
        if(false)  // determine path as valid (scan contents and parse for correctness)
            return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    QApplication openPilotLog(argc, argv);
    QCoreApplication::setOrganizationName("openPilotLog");
    QCoreApplication::setOrganizationDomain("https://github.com/fiffty-50/openpilotlog");
    QCoreApplication::setApplicationName("openPilotLog");

    StandardAppPaths std_paths;
    scan_paths(std_paths);
    if(!validate_paths(std_paths)){
        DEB("Standard paths not valid.");
        return 1;
    }

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, std_paths.paths[QStandardPaths::AppDataLocation]);
    QSettings settings;

    experimental::aDB()->connect();

//    if (!ASettings::read("setup/setup_complete").toBool()) {
//        FirstRunDialog dialog;
//        dialog.exec();
//    }

    //Theming
//    int selectedtheme = settings.value("main/theme").toInt();
//    QDir::setCurrent("/themes");
    switch (2) {
    case 1:{
        qDebug() << "main :: Loading light theme";
        QFile file(":light.qss");
        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream stream(&file);
        openPilotLog.setStyleSheet(stream.readAll());
        break;
    }
    case 2:{
        qDebug() << "Loading dark theme";
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
    ARunGuard guard("opl_single_key");
        if ( !guard.tryToRun() ){
            qDebug() << "Another Instance is already running. Exiting.";
            return 0;
        }

    MainWindow w;
    //w.showMaximized();
    w.show();
    return openPilotLog.exec();
}
