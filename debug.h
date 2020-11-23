#ifndef DEBUG_H
#define DEBUG_H

// Debug Makro
#define DEB(expr) \
    qDebug() << __PRETTY_FUNCTION__ << "\t" << expr

#endif // DEBUG_H
