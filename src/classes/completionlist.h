#ifndef COMPLETIONLIST_H
#define COMPLETIONLIST_H

#include <QCoreApplication>
#include "src/database/db.h"

class completerTarget
{
public:
    enum targets {airports, pilots, registrations, aircraft};
};


/*!
 * \brief The completionList class provides QStringLists to be used by a QCompleter
 */
class completionList
{
public:



    QStringList list;

    completionList();

    completionList(completerTarget::targets);
};

#endif // COMPLETIONLIST_H
