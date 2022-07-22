#ifndef PATHS_H
#define PATHS_H

#include <QCoreApplication>
#include <QDir>
#include <QHash>


namespace OPL {


class Paths
{
public:
    enum Directories {
        Database,
        Templates,
        Backup,
        Log,
    };

    const static inline QHash<Directories, QLatin1String> directories = {
        {Database, QLatin1String("/database")},
        {Templates, QLatin1String("/templates")},
        {Backup, QLatin1String("/backup")},
        {Log, QLatin1String("/log")},
    };

    Paths();

    static const bool setup();

    /*!
     * \brief Returns the QDir for the standard directory referenced
     * by the Directories enum 'loc'
     */
    static const QDir directory(Directories location);

    static const QString path(Directories location);

    static const QString appDir() {return QCoreApplication::applicationDirPath();}

    static const QString filePath(Directories location, const QString &filename);

    /*!
     * \brief returns a QFileInfo for the default database file.
     */
    static const QFileInfo databaseFileInfo();
};


} // namespace OPL
#endif // PATHS_H
