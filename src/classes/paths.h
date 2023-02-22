#ifndef PATHS_H
#define PATHS_H

#include <QCoreApplication>
#include <QDir>
#include <QHash>
#include <QStandardPaths>
#include "src/opl.h"


namespace OPL {

/*!
 * \brief The Paths class provides paths for the applications folder structure.
 * \details In order to be cross-platform compatible, QStandardPaths returns the default
 * writable locations for Application Data for the current operating system,
 * for example `/home/user/.local/share/opl` on linux or `/Users/user/Library/Application Support/opl` on macos.
 *
 * OPL writes into a singular directory, with several subdirectories, which are enumerated in Directories.
 */
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

    /*!
     * \brief the base Path of the Application Directory. Evaluates to an XDG writable App Data location depending on the OS
     */
    static const inline QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + QDir::toNativeSeparators("/")
            + ORGNAME + QDir::toNativeSeparators("/");
};


} // namespace OPL
#endif // PATHS_H
