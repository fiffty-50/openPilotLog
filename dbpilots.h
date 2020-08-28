#ifndef DBPILOTS_H
#define DBPILOTS_H

#include <QCoreApplication>

/*!
 * \brief The dbPilots class class provides a databank interface for actions related to the
 * pilots table.
 */
class dbPilots
{
public:

    static QString retreivePilotNameFromID(QString pilotID);

    static QString retreivePilotIdFromString(QString lastname, QString firstname);

    static QStringList retreivePilotNameFromString(QString searchstring);

    static QStringList newPicGetString(QString searchstring);

    static QString newPicGetId(QString name);
};

#endif // DBPILOTS_H
