#ifndef ALOG_H
#define ALOG_H

#include <QTime>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <iostream>
#include <QDebug>

#if defined(__GNUC__) || defined(__clang__)
    #define FUNC_IDENT __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define FUNC_IDENT __FUNCSIG__
#else
    #define FUNC_IDENT __func__
#endif

#define DEB qDebug() << FUNC_IDENT << "\n\t"    // Use for debugging
#define LOG qInfo()                             // Use for logging milestones (silently, will be written to log file and console out only)
#define INFO qInfo()                            // Use for messages of interest to the user (will be displayed in GUI)
#define WARN qWarning()                         // Use for warnings (will be displayed in GUI)
#define CRIT qCritical()                        // Use for critical warnings (will be displayed in GUI)
#define TODO qCritical() << "!\n\tTo Do:\t"

/*!
 * \brief The ALog namespace encapsulates constants and functions used to provide logging to files
 * and logging to console (stdout)
 *
 * \abstract
 *
 * The console output is color coded - green, amber, magenta for info, warn and crit messages, whereas
 * the log files are just plain text.
 *
 * There is a maximum of <numberOfLogs> log files with a maximum size of <sizeOfLogs>,
 * at the moment, up to 10 logs of up to 100kB in size are kept, older logs are
 * automatically deleted.
 *
 * Debug output is not written to the logfile.
 *
 * In order to start logging, the ALog::init() function has to be called
 *
 * Credits to [Andy Dunkel](https://andydunkel.net/) for his excellent blog post on Qt Log File Rotation!
 */
namespace ALog
{
    static QString logFolderName = "logs"; // To Do: AStandardpaths
    static QString logFileName;
    const static int numberOfLogs = 10; // max number of log files to keep
    const static int sizeOfLogs = 1024 * 100; // max log size in bytes, = 100kB

    const static auto DEB_HEADER  = QLatin1String("\n[OPL - Deb ]: ");
    const static auto INFO_HEADER = QLatin1String(" [OPL - INFO]: ");
    const static auto WARN_HEADER = QLatin1String(" [OPL - WARN]: ");
    const static auto CRIT_HEADER = QLatin1String(" [OPL - CRIT]: ");
    const static auto INFO_HEADER_CONSOLE = QLatin1String("\033[32m[OPL - INFO]: \033[m");
    const static auto WARN_HEADER_CONSOLE = QLatin1String("\033[33m[OPL - WARN]: \033[m");
    const static auto CRIT_HEADER_CONSOLE = QLatin1String("\033[35m[OPL - CRIT]: \033[m");

    bool init();
    void setLogFileName();
    void deleteOldLogs();
    void aMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString& msg);
} // namespace ALog

/*!
 * Representation macro for custom classes.
 *
 * Usage
 * -----
 * class Myclass {
 *  ...
 * 	REPR(MyClass,
 *       "member1=" + object.member1 + ", "
 *       "something2" + object.calculate()
 *      )
 * };
 *
 * MyClass mc;
 * DEB << mc;
 *
 * output:
 * MyClass(member1=3000, something2="A320")
 */
#define REPR(cls, str) \
friend \
QDebug operator<<(QDebug qdb, const cls& object) \
{ \
    qdb << QString(#cls) + '(' + str + ')'; \
    return qdb; \
}

#endif // ALOG_H
