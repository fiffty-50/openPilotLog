#ifndef ADEBUG_H
#define ADEBUG_H

#include <QDebug>

#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr

#endif
