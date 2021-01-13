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
 * `QCoreApplication::setWhateverName`. The paths contained in this class
 * include a trailing seperator and all seperators are platform-agnostic.
 */
class AStandardPaths{
public:
    enum Directories {
        Database,
        Templates,
        DatabaseBackup
    };
private:
    static QMap<Directories, QString> directories;
public:
    /// Initialise paths with corresponding StandardLocation paths
    static void setup();

    // [G]: Subjective opinion:
    // We should move away from getThis getThat functions.
    // I believe we can give better namings while avoiding this
    // OOP cliche of getEverything
    static const QString& absPathOf(Directories loc);
    static const QMap<Directories, QString>& allPaths();

    /// Ensure standard app directories exist, if not mkpath them.
    static void scan_dirs();

    /// Validate standard app directories are valid in structure and contents.
    static bool validate_dirs();
};


#endif // ASTANDARDPATHS_H
