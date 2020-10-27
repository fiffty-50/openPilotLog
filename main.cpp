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
#include <QApplication>
#include <QProcess>
#include <QSettings>

int main(int argc, char *argv[])
{

    QCoreApplication::setOrganizationName("openPilotLog");
    QCoreApplication::setOrganizationDomain("https://github.com/fiffty-50/openpilotlog");
    QCoreApplication::setApplicationName("openPilotLog");


    if(!QDir("data").exists()){
        QDir().mkdir("data");
    }
    QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,"data");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings settings;

    QApplication openPilotLog(argc, argv);

    //Theming
    int selectedtheme = settings.value("main/theme").toInt();
    QDir::setCurrent("/themes");

    switch (selectedtheme) {
    case 1:
    {
        qDebug() << "main :: Loading light theme";
        QFile file(":light.qss");
        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream stream(&file);
        openPilotLog.setStyleSheet(stream.readAll());
        break;
    }
    case 2:
        qDebug() << "Loading dark theme";
        QFile file(":dark.qss");
        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream stream(&file);
        openPilotLog.setStyleSheet(stream.readAll());
        break;
    }

    MainWindow w;
    w.show();
    return openPilotLog.exec();
}
