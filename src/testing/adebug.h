#ifndef ADEBUG_H
#define ADEBUG_H

#include <QDebug>

#if defined(__GNUC__) || defined(__clang__)
    #define FUNC_IDENT __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define FUNC_IDENT __FUNCSIG__
#else
    #define FUNC_IDENT __func__
#endif

// CAUTION: qDebug() doesnt print for non-DEBUG builds
#define DEB qDebug() << FUNC_IDENT << "\n\t"
#define DEB_SRC DEB
#define DEB_RAW qDebug() << '\t'


// [G]: TODO study cross platform terminal coloring
// might be silly but coloring specific words does increase
// ease of reading debug output. We dont have to go overboard
// start with the header.
// DRAFT:
// info -> white
// warning -> yellow
// critical -> red
// there is also fatal which even kills the program and could be purple.
#define INFO qInfo() << "info:"
#define WARN qWarning() << "warning:"
#define CRIT qCritical() << "critical:"
#define NOT_IMPLEMENTED qCritical() << FUNC_IDENT << "\n\t" << "~~ NOT IMPLEMENTED ~~";

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

#endif
