#include "alog.h"

namespace ALog {

/*!
 * \brief setLogFileName sets a log file name ("Log_<Date>_<Time>.txt")
 */
void setLogFileName()
{
    logFileName = QString(logFolderName + QLatin1String("/Log_%1_%2.txt")
                          ).arg(QDate::currentDate().toString(QStringLiteral("yyyy_MM_dd")),
                                QTime::currentTime().toString(QStringLiteral("hh_mm_ss")));
}

/*!
 * \brief Cleans up old logs and initializes logging by preparing and installing a QMessageHandler
 *
 */
void deleteOldLogs()
{
    if(!QDir(logFolderName).exists()) {
        QDir().mkdir(logFolderName);
    }

    QDir dir;
    dir.setSorting(QDir::Time | QDir::Reversed);
    dir.setPath(logFolderName);

    QFileInfoList logs_list = dir.entryInfoList();
    if (logs_list.size() <= numberOfLogs) {
        return;
    } else {
        for (int i = 0; i < (logs_list.size() - numberOfLogs); i++) {
            const QString path = logs_list.at(i).absoluteFilePath();
            QFile file(path);
            file.remove();
        }
    }
}

/*!
 * \brief initialise logging, clean up logfiles and install a QMessageHandler
 */
bool init()
{
    // Create folder for logfiles if it doesn't exist, move to AStandardPaths later
    if(!QDir(logFolderName).exists()) {
        QDir().mkdir(logFolderName);
    }

    deleteOldLogs();
    setLogFileName();

    QFile log_file((logFileName));
    if(log_file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qInstallMessageHandler(ALog::aMessageHandler);
        return true;
    } else {
        return false;
    }
}

/*!
 * \brief aMessageHandler Intercepts Messages and prints to console and log file
 *
 * \abstract The message handler is responsible for intercepting the output from
 * qDebug(), qInfo(), qWarning() and qCritical(), formatting them and printing them
 * to the standard console out and to a logfile using QTextStream. Debug messages are
 * not written to the log file.
 *
 */
void aMessageHandler(QtMsgType type, const QMessageLogContext &context,
                      const QString& msg)
{
    const char *function = context.function ? context.function : "";
    //check file size and if needed create new log!
    {
        QFile outFileCheck(logFileName);
        int size = outFileCheck.size();

        if (size > sizeOfLogs) {
            deleteOldLogs();
            setLogFileName();
        }
    }
    // open the log file and prepare a textstream to write to it
    QFile log_file(logFileName);
    log_file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream log_stream(&log_file);

    switch (type) {
        case QtDebugMsg:
            QTextStream(stdout) << "\t\033\[31m" << function << DEB_HEADER << msg << "\033[m" << endl;
            break;
        case QtInfoMsg:
            log_stream << QTime::currentTime().toString(Qt::ISODate) << INFO_HEADER << msg << function << endl;
            QTextStream(stdout) << INFO_HEADER_CONSOLE << msg  << endl;
            break;
        case QtWarningMsg:
            log_stream << QTime::currentTime().toString(Qt::ISODate) << WARN_HEADER << msg << function << endl;
            QTextStream(stdout) << WARN_HEADER_CONSOLE << msg << endl;
            break;
        case QtCriticalMsg:
            log_stream << QTime::currentTime().toString(Qt::ISODate) << CRIT_HEADER << msg << function << endl;
            QTextStream(stdout) << CRIT_HEADER_CONSOLE << msg << endl;
            break;
    default:
            log_stream << QTime::currentTime().toString(Qt::ISODate) << INFO_HEADER << msg << function << endl;
            QTextStream(stdout) << INFO_HEADER_CONSOLE << msg << endl;
            break;
        }
}

} // namespace ALog
