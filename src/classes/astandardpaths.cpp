#include "src/classes/astandardpaths.h"

//QMap<QStandardPaths::StandardLocation, QString> AStandardPaths::paths;
QMap<AStandardPaths::Dirs, QString> AStandardPaths::paths;

void AStandardPaths::setup()
{
    auto settings_location = QStandardPaths::AppConfigLocation;
    auto data_location = QStandardPaths::AppDataLocation;
    paths = {  // [G]: potential rename to `dirs`
        {Database, QStandardPaths::writableLocation(data_location)},
        {Templates, QDir(QStandardPaths::writableLocation(data_location)).filePath("templates")},
        {Settings, QStandardPaths::writableLocation(settings_location)},
        {DatabaseBackup, QDir(QStandardPaths::writableLocation(data_location)).filePath("backup")}
    };
    DEB << "Paths created: " << paths;
}

QString AStandardPaths::pathTo(Dirs loc)
{
    return paths[loc];
}

QMap<AStandardPaths::Dirs, QString> AStandardPaths::allPaths()
{
    return paths;
}

void AStandardPaths::scan_paths()
{
    for(auto& path : paths){
        auto dir = QDir(path);
        DEB << "Scanning " << dir.path();
        if(!dir.exists()) {
            DEB <<"Creating " << dir.path();
            dir.mkpath(path);
        }
    }
}

bool AStandardPaths::validate_paths()
{
    for(auto& path : paths){
        DEB << "Validating " << path;
        if(false)  // determine path as valid (scan contents and parse for correctness)
            return false;
    }
    return true;
}
