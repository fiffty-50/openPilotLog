#include "src/astandardpaths.h"

//QMap<QStandardPaths::StandardLocation, QString> AStandardPaths::paths;
QMap<AStandardPaths::Dirs, QString> AStandardPaths::paths;

void AStandardPaths::setup()
{
    auto settings_location = QStandardPaths::AppConfigLocation;
    auto data_location = QStandardPaths::AppDataLocation;
    paths = {
        {Database, QStandardPaths::writableLocation(data_location)},
        {Templates, QDir(QStandardPaths::writableLocation(data_location)).filePath("templates")},
        {Settings, QStandardPaths::writableLocation(settings_location)},
    };
    DEB << "Paths created: " << paths;
}

QString AStandardPaths::getPath(Dirs loc)
{
    return paths[loc];
}

QMap<AStandardPaths::Dirs, QString> AStandardPaths::getPaths()
{
    return paths;
}

void AStandardPaths::scan_paths()
{
    for(auto& path : paths.values()){
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
    for(auto& path : paths.values()){
        DEB << "Validating " << path;
        if(false)  // determine path as valid (scan contents and parse for correctness)
            return false;
    }
    return true;
}
