#include "src/classes/astandardpaths.h"

QMap<AStandardPaths::Directories, QString> AStandardPaths::directories;

void AStandardPaths::setup()
{
    auto data_location = QStandardPaths::AppDataLocation;
    directories = { // [F]: Dir could be ambiguous since it is very similar to QDir
        {Database, QDir::toNativeSeparators(
         QStandardPaths::writableLocation(data_location) + '/')},
        {Templates, QDir::toNativeSeparators(
         QDir(QStandardPaths::writableLocation(data_location)).filePath(QStringLiteral("templates/")))},
        {DatabaseBackup, QDir::toNativeSeparators(
         QDir(QStandardPaths::writableLocation(data_location)).filePath(QStringLiteral("backup/")))}
    };
}

const QString& AStandardPaths::absPathOf(Directories loc)
{
    return directories[loc];
}

const QMap<AStandardPaths::Directories, QString>& AStandardPaths::allPaths()
{
    return directories;
}

void AStandardPaths::scan_dirs()
{
    for(auto& dir : directories){
        auto d = QDir(dir);
        if(!d.exists()) {
            d.mkpath(dir);
        }
    }
}

// [F]: We could make scan_dirs return bool and return false if !exists && !mkpath
// This function seems like it would do the same as scan_dirs
// Validating the contents would be rather complex to accomplish and difficult to
// maintain I believe, since the contents of these directories might change and it
// seems out of the scope of this class to verify the directories' contents.
bool AStandardPaths::validate_dirs()
{
    for(auto& dir : directories){
        //DEB << "Validating " << dir;
        if(false)  // determine path as valid (scan contents and parse for correctness)
            return false;
    }
    return true;
}
