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
