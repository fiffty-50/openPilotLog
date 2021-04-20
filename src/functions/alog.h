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
#include "src/classes/astandardpaths.h"

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
    static QDir logFolder;
    static QString logFileName;
    const static int numberOfLogs = 10; // max number of log files to keep
    const static int sizeOfLogs = 1024 * 100; // max log size in bytes, = 100kB

    const static auto DEB_HEADER  = QLatin1String(" [OPL - Deb ]: ");
    const static auto INFO_HEADER = QLatin1String(" [OPL - INFO]: ");
    const static auto WARN_HEADER = QLatin1String(" [OPL - WARN]: ");
    const static auto CRIT_HEADER = QLatin1String(" [OPL - CRIT]: ");
    const static auto DEB_HEADER_CONSOLE  = QLatin1String("\u001b[38;5;69m[OPL - Deb ]: ");
    const static auto INFO_HEADER_CONSOLE = QLatin1String("\033[32m[OPL - INFO]: \033[m");
    const static auto WARN_HEADER_CONSOLE = QLatin1String("\033[33m[OPL - WARN]: \033[m");
    const static auto CRIT_HEADER_CONSOLE = QLatin1String("\033[35m[OPL - CRIT]: \033[m");

    bool init(bool log_debug = false);
    void setLogFileName();
    void deleteOldLogs();
    void aMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString& msg);
    inline static const QString timeNow(){return QTime::currentTime().toString(Qt::ISODate);}

    /*!
     * \brief info - Informs the user of an important program milestone
     * \param msg - the message to be displayed. Shall be a translatable string (tr)
     * \abstract This function is used to inform the user about an important step the
     * program has (successfully) completed. This is achieved by displaying a QMessageBox.
     *
     * This function also creates a qInfo message, which will be written to the logfile and
     * to stdout
     */
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
