#ifndef DBSTAT_H
#define DBSTAT_H

#include <QCoreApplication>


/*!
 * \brief The dbStat class provides functionality for retreiving various statistics
 * from the database. In general, most values are provided as either QString or
 * QVector<QString>.
 *
 */
class dbStat
{
public:
    dbStat();
    static QString retreiveTotalTime();
    static QString retreiveTotalTimeThisCalendarYear();
    static QString retreiveTotalTimeRollingYear();
    static QVector<QString> retreiveCurrencyTakeoffLanding();
};

#endif // DBSTAT_H
