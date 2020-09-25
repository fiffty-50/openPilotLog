#ifndef DBAIRCRAFT_H
#define DBAIRCRAFT_H

#include <QCoreApplication>

/*!
 * \brief The dbAircraft class provides functionality for retreiving and storing
 * data relateted to the aircraft and tails tables of the database.
 */
class dbAircraft
{
public:
    static QString retreiveRegistration(QString tail_ID);

    static QStringList retreiveRegistrationList();

    static QStringList newAcftGetString(QString searchstring);

    static QString retreiveTailId(QString registration);

    static QString retreiveAircraftId(QString tail_id);

    static QVector<QString> retreiveAircraftTypeFromReg(QString searchstring);

    static QStringList retreiveAircraftMake(QString searchstring);

    static QStringList retreiveAircraftModel(QString make, QString searchstring);

    static QStringList retreiveAircraftVariant(QString make, QString model, QString searchstring);

    static QString retreiveAircraftIdFromMakeModelVariant(QString make, QString model, QString variant);

    static bool commitTailToDb(QString registration, QString aircraft_id, QString company);

    static QVector<QString> retreiveAircraftDetails(QString aircraft_id);

};

#endif // DBAIRCRAFT_H
