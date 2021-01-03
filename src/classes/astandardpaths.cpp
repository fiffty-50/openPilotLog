#include "src/classes/astandardpaths.h"

//QMap<QStandardPaths::StandardLocation, QString> AStandardPaths::paths;
QMap<AStandardPaths::Dirs, QString> AStandardPaths::dirs;

void AStandardPaths::setup()
{
    auto settings_location = QStandardPaths::AppConfigLocation;
    auto data_location = QStandardPaths::AppDataLocation;
    dirs = {  // [G]: potential rename to `dirs`
        {Database, QStandardPaths::writableLocation(data_location)},
        {Templates, QDir(QStandardPaths::writableLocation(data_location)).filePath("templates")},
        {Settings, QStandardPaths::writableLocation(settings_location)},
        {DatabaseBackup, QDir(QStandardPaths::writableLocation(data_location)).filePath("backup")}
    };
    DEB << "Paths created: " << dirs;
}

const QString& AStandardPaths::absPathOf(Dirs loc)
{
    return dirs[loc];
}

const QMap<AStandardPaths::Dirs, QString>& AStandardPaths::allPaths()
{
    return dirs;
}

void AStandardPaths::scan_dirs()
{
    for(auto& dir : dirs){
        auto d = QDir(dir);
        DEB << "Scanning " << d.path();
        if(!d.exists()) {
            DEB <<"Creating " << d.path();
            d.mkpath(dir);
        }
    }
}

bool AStandardPaths::validate_dirs()
{
    for(auto& dir : dirs){
        DEB << "Validating " << dir;
        if(false)  // determine path as valid (scan contents and parse for correctness)
            return false;
    }
    return true;
}
