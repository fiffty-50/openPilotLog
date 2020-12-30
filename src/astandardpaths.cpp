#include "src/astandardpaths.h"

QMap<QStandardPaths::StandardLocation, QString> AStandardPaths::paths;

void AStandardPaths::setup()
{
     paths = {
        {QStandardPaths::AppConfigLocation, QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)},
        {QStandardPaths::AppDataLocation, QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)},
    };
}

QMap<QStandardPaths::StandardLocation, QString> AStandardPaths::getPaths()
{
    return paths;
}

void AStandardPaths::scan_paths()
{
    for(auto& path : paths.values()){
        auto dir = QDir(path);
        DEB "Scanning " << dir.path();
        if(!dir.exists()) {
            DEB"Creating " << dir.path();
            dir.mkpath(path);
        }
    }
}

bool AStandardPaths::validate_paths()
{
    for(auto& path : paths.values()){
        DEB "Validating " << path;
        if(false)  // determine path as valid (scan contents and parse for correctness)
            return false;
    }
    return true;
}
