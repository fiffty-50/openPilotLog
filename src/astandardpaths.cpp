#include "src/astandardpaths.h"

QMap<QStandardPaths::StandardLocation, QString> AStandardPaths::paths;

void AStandardPaths::setup()
{
    auto settings_location = QStandardPaths::AppConfigLocation;
    auto data_location = QStandardPaths::AppDataLocation;
    paths = {
        {settings_location, QStandardPaths::writableLocation(settings_location)},
        {data_location, QStandardPaths::writableLocation(data_location)},
    };
}

QString AStandardPaths::getPath(QStandardPaths::StandardLocation loc)
{
    return paths[loc];
}

QMap<QStandardPaths::StandardLocation, QString> AStandardPaths::getPaths()
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
