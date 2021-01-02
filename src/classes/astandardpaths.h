#ifndef ASTANDARDPATHS_H
#define ASTANDARDPATHS_H

#include "src/testing/adebug.h"
#include <QStandardPaths>
#include <QString>
#include <QMap>
#include <QDir>

/*!
 * \brief The AStandardAppPaths class encapsulates a static QMap holding
 * the standard paths of the application. Setup should be called after
 * `QCoreApplication::setWhateverName`.
 */
class AStandardPaths{
public:
    enum Dirs {
        Database,
        Templates,
        Settings,
        DatabaseBackup
    };
private:
    static
    QMap<Dirs, QString> paths;
public:
    /// Initialise paths with corresponding StandardLocation paths
    static void setup();

    // [G]: Subjective opinion:
    // We should move away from getThis getThat functions.
    // I believe we can give better namings while avoiding this
    // OOP cliche of getEverything
    static QString pathTo(Dirs loc);
    static QMap<Dirs, QString> allPaths();

    /// Ensure standard app paths exist, if not mkdir them.
    static void scan_paths();

    /// Validate standard app paths are valid in structure and contents.
    static bool validate_paths();
};


#endif // ASTANDARDPATHS_H
