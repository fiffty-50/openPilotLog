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


const auto DATA_DIR = QLatin1String("data");
/*!
 * \brief setup checks if data folder and settings files exists.
 * \return
 */
bool setup()
{
    if (!QDir(DATA_DIR).exists())
        QDir().mkdir(DATA_DIR);

    QDir      settingspath(DATA_DIR + QLatin1Char('/') + QCoreApplication::organizationName());
    QString   settingsfile = QCoreApplication::applicationName() + QLatin1String(".ini");
    QFileInfo check_file(settingspath,settingsfile);

    QSettings settings;
    settings.setValue("setup/touch", true);
    settings.sync();

    return check_file.exists() && check_file.isFile();
};

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("openPilotLog");
    QCoreApplication::setOrganizationDomain("https://github.com/fiffty-50/openpilotlog");
    QCoreApplication::setApplicationName("openPilotLog");

    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, DATA_DIR);
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    experimental::aDB()->connect();

    QApplication openPilotLog(argc, argv);
    if(!setup()){
        DEB("error creating required directories");
        return 0;
    }

    if (!ASettings::read("setup/setup_complete").toBool()) {
        FirstRunDialog dialog;
        dialog.exec();
    }



    //Theming
    int selectedtheme = settings.value("main/theme").toInt();
    QDir::setCurrent("/themes");
    switch (selectedtheme) {
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
