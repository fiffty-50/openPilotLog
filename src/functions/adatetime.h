#ifndef ADATETIME_H
#define ADATETIME_H
#include <QtCore>
#include "src/oplconstants.h"

namespace ADateTime {

/*!
 * \brief toString formats a QDateTime object into a string in a uniform way.
 * \return
 */
inline const QString toString (const QDateTime date_time, Opl::Datetime::DateTimeFormat format) {
    switch (format) {
    case Opl::Datetime::Default:
        return date_time.toString(Qt::ISODate);
    case Opl::Datetime::Backup:
        return date_time.toString(QStringLiteral("yyyy_MM_dd_T_hh_mm"));
    default:
        return QString();
    }
}

}

#endif // ADATETIME_H
