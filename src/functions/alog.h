/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
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

    const static auto DEB_HEADER  = QLatin1String(" [DEBG]:\t");
    const static auto INFO_HEADER = QLatin1String(" [INFO]:\t");
    const static auto WARN_HEADER = QLatin1String(" [WARN]:\t");
    const static auto CRIT_HEADER = QLatin1String(" [CRIT]:\t");
    const static auto DEB_HEADER_CONSOLE  = QLatin1String("\u001b[38;5;75m[DEBG]:\t");
    const static auto INFO_HEADER_CONSOLE = QLatin1String("\033[32m[INFO]:\t\033[m");
    const static auto WARN_HEADER_CONSOLE = QLatin1String("\033[33m[WARN]:\t\033[m");
    const static auto CRIT_HEADER_CONSOLE = QLatin1String("\033[35m[CRIT]:\t\033[m");
    const static auto SPACER = QLatin1String("\t\t");
    const static auto D_SPACER = QLatin1String("\t\t\t\t");

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
