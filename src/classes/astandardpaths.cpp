#include "src/classes/astandardpaths.h"

QMap<AStandardPaths::Dirs, QString> AStandardPaths::dirs;

void AStandardPaths::setup()
{
    auto data_location = QStandardPaths::AppDataLocation;
    dirs = {  // [G]: potential rename to `dirs`
        {Database, QStandardPaths::writableLocation(data_location)},
        {Templates, QDir(QStandardPaths::writableLocation(data_location)).filePath("templates")},
        {DatabaseBackup, QDir(QStandardPaths::writableLocation(data_location)).filePath("backup")}
    };
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
        if(!d.exists()) {
            d.mkpath(dir);
        }
    }
}

bool AStandardPaths::validate_dirs()
{
    for(auto& dir : dirs){
        //DEB << "Validating " << dir;
        if(false)  // determine path as valid (scan contents and parse for correctness)
            return false;
    }
    return true;
}
