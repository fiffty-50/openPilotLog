#ifndef PATHS_H
#define PATHS_H

#include <QCoreApplication>
#include <QDir>
#include <QHash>
#include <QStandardPaths>
#include "src/opl.h"


namespace OPL {


class Paths
{
public:
    Paths() = delete;

    enum Directories {
        Database,
        Templates,
        Backup,
        Log,
        Export,
        Settings,
    };

    const static inline QHash<Directories, QLatin1String> directories = {
        {Database, QLatin1String("/database")},
        {Templates, QLatin1String("/templates")},
        {Backup, QLatin1String("/backup")},
        {Log, QLatin1String("/log")},
        {Export, QLatin1String("/export")},
        {Settings, QLatin1String("/settings")},
    };

    static const bool setup();

    /*!
     * \brief Returns the QDir for the standard directory referenced
     * by the Directories enum 'loc'
     */
    static const QDir directory(Directories location);

    static const QString path(Directories location);

    static const QString filePath(Directories location, const QString &filename);

    /*!
     * \brief returns a QFileInfo for the default database file.
     */
    static const QFileInfo databaseFileInfo();
private:
    // Define the paths where the application data will be written. This will be standard locations on all platforms eventually
    // but for now on Windows and MacOS, we use the application runtime directory to make it easier to
    // debug and develop. On Linux XDG standard directories are required for the flatpak to work.
#ifdef linux
    static const inline QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + QDir::toNativeSeparators("/")
            + ORGNAME + QDir::toNativeSeparators("/");
#else
    static const inline QString basePath = QCoreApplication::applicationDirPath();
#endif

};


} // namespace OPL
#endif // PATHS_H
