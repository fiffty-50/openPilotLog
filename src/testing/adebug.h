#ifndef ADEBUG_H
#define ADEBUG_H

#include <QDebug>

#define DEB qDebug() << __PRETTY_FUNCTION__ << "\t" <<

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
 * DEB mc;
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
