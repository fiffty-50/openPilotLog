#include "paths.h"
#include "src/opl.h"

namespace OPL {

Paths::Paths()
{

}

const bool Paths::setup()
{

// Define the application paths. This will be standard locations on all platforms eventually
// but for now on Windows and MacOS use the application runtime directory to make it easier to
// debug and develop. On Linux XDG standard directories are required for the flatpak to work.

#ifdef linux
    LOG << "Setting up directories at: " << QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    const QString dir_path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    for(const auto& str : qAsConst(directories)){
        QDir dir(dir_path + str);
        if(!dir.exists()) {
            if (!dir.mkpath(dir.absolutePath()))
                return false;
        }
    }
    return true;
#else
    LOG << "Setting up directories at: " << QCoreApplication::applicationDirPath();
    const QString dir_path = QCoreApplication::applicationDirPath();
    for(const auto& str : qAsConst(directories)){
        QDir dir(dir_path + str);
        if(!dir.exists()) {
            if (!dir.mkpath(dir.absolutePath()))
                return false;
        }
    }
    return true;
#endif
}

const QDir Paths::directory(Directories location)
{
    return QDir(QCoreApplication::applicationDirPath() + directories[location]);
}

const QString Paths::path(Directories location)
{
    return QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + directories[location]);
}

const QString Paths::filePath(Directories location, const QString &filename)
{
    QDir dir(QCoreApplication::applicationDirPath() + directories[location]);
    return dir.absoluteFilePath(filename);
}

const QFileInfo Paths::databaseFileInfo()
{
    return QFileInfo(directory(Database), QStringLiteral("logbook.db"));
}

} // namespace OPL
