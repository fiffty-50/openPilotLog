#ifndef ADEBUG_H
#define ADEBUG_H

#include <QDebug>

// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr

#endif // ADEBUG_H
