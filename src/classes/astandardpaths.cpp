#include "src/classes/astandardpaths.h"

QMap<AStandardPaths::Directories, QDir> AStandardPaths::directories;

void AStandardPaths::setup()
{
    auto data_location = QStandardPaths::AppDataLocation;
    directories = { // [F]: Dir could be ambiguous since it is very similar to QDir
        {Database, QDir(QStandardPaths::writableLocation(data_location) + '/')},
        {Templates, QDir(QStandardPaths::writableLocation(data_location)).filePath(
         QStringLiteral("templates/"))},
        {Backup, QDir(QStandardPaths::writableLocation(data_location)).filePath(
         QStringLiteral("backup/"))}
    };
}

const QDir& AStandardPaths::directory(Directories loc)
{
    return directories[loc];
}

const QMap<AStandardPaths::Directories, QDir>& AStandardPaths::allDirectories()
{
    return directories;
}

bool AStandardPaths::scan_dirs()
{
    for(const auto& dir : directories){
        if(!dir.exists()) {
            DEB << dir << "Does not exist. Creating: " << dir.absolutePath();
            if (!dir.mkpath(dir.absolutePath()))
                return false;
        }
    }
    return true;
}
