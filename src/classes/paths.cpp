#include "paths.h"
#include "src/opl.h"

namespace OPL {

Paths::Paths()
{

}

const bool Paths::setup()
{
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
