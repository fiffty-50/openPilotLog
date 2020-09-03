#include "dbaircraft.h"
#include "dbman.cpp"



/*!
 * \brief RetreiveRegistration Looks up tail_id from Database
 * \param tail_ID Primary Key of tails database
 * \return Registration
 */
QString dbAircraft::retreiveRegistration(QString tail_ID)

{
    QString acftRegistration("");

    QSqlQuery query;
    query.prepare("SELECT registration FROM tails WHERE tail_id == ?");
    query.addBindValue(tail_ID.toInt());
    query.exec();

    if(query.first());
    else
        qDebug() << ("No Aircraft with this ID found");
    query.previous();//To go back to index 0
    while (query.next()) {
        acftRegistration.append(query.value(0).toString());
    }

    return acftRegistration;
}

QStringList dbAircraft::retreiveRegistrationList()
{
    QSqlQuery query;
    query.prepare("SELECT registration FROM tails");
    query.exec();

    QStringList result;
    while (query.next()) {
        result.append(query.value(0).toString());
    }
    return result;
}

/*!
 * \brief newAcftGetString Looks up an aircraft Registration in the database
 * \param searchstring
 * \return Registration, make, model and variant
 */
QStringList dbAircraft::newAcftGetString(QString searchstring)
{
    QStringList result;
    if(searchstring.length()<2){return result;}
    QSqlQuery query;
    query.prepare("SELECT registration, make, model, variant "
                  "FROM aircraft "
                  "INNER JOIN tails on tails.aircraft_ID = aircraft.aircraft_id "
                  "WHERE tails.registration LIKE ?");
    searchstring.append("%"); searchstring.prepend("%");
    query.addBindValue(searchstring);
    query.exec();

    while(query.next())
    {
        result.append(query.value(0).toString() + " (" + query.value(1).toString() + "-" + query.value(2).toString() + "-" + query.value(3).toString() + ")");
    }
    qDebug() << "newAcftGetString: " << result.length() << result;
    return result;

}
/*!
 * \brief dbAircraft::newAcftGetId Looks up a registration in the databse
 * \param registration Aircraft Registration
 * \return tail_id
 */
QString dbAircraft::retreiveTailId(QString registration)
{
    QString result;
    QSqlQuery query;
    query.prepare("SELECT tail_id "
                  "FROM tails "
                  "WHERE registration LIKE ?");
    registration.prepend("%"); registration.append("%");
    query.addBindValue(registration);
    query.exec();

    while(query.next())
    {
        result.append(query.value(0).toString());
    }
    qDebug() << "retreiveTailId: " << result;
    return result;

}

/*!
 * \brief dbAircraft::retreiveAircraftId Looks up aircraft_id in tails table
 * \param tail_id
 * \return aircraft_id
 */
QString dbAircraft::retreiveAircraftId(QString tail_id)
{
    QString result;
    QSqlQuery query;
    query.prepare("SELECT aircraft_id "
                  "FROM tails "
                  "WHERE tail_id = ?");
    query.addBindValue(tail_id);
    query.exec();

    while(query.next())
    {
        result.append(query.value(0).toString());
    }
    qDebug() << "retreiveAircraftId: " << result;
    return result;
}
/*!
 * \brief dbAircraft::retreiveAircraftTypeFromReg Searches the tails Database
 * \param searchstring
 * \return {registration, type, iata Code, tail_id} or {}
 */
QVector<QString> dbAircraft::retreiveAircraftTypeFromReg(QString searchstring)
{

    QSqlQuery query;
    query.prepare("SELECT Name, iata, registration, tail_id " //"SELECT Registration, Name, icao, iata "
                  "FROM aircraft "
                  "INNER JOIN tails on tails.aircraft_ID = aircraft.aircraft_id "
                  "WHERE tails.registration LIKE ?");
    // Returns Registration/Name/icao/iata
    searchstring.prepend("%");
    searchstring.append("%");
    query.addBindValue(searchstring);
    query.exec();

    QVector<QString> result;
    if(query.first())
    {
        QString acType = query.value(0).toString();
        QString iataCode = query.value(1).toString();
        QString registration = query.value(2).toString();
        QString tail_id = query.value(3).toString();
        result.append(registration); result.append(acType);
        result.append(iataCode); result.append(tail_id);
        return result;
    }else
    {
        return  result; // return empty
    }
}

/*!
 * \brief dbAircraft::retreiveAircraftMake Search function to provide a
 * QStringList to the QCompleter
 * \param searchstring A possible aircraft manufacturer
 * \return Possible values according to the aircraft database
 */
QStringList dbAircraft::retreiveAircraftMake(QString searchstring)
{
    QStringList result;
    QSqlQuery query;
    query.prepare("SELECT make from aircraft WHERE make LIKE ?");
    searchstring.prepend("%"); searchstring.append("%");
    query.addBindValue(searchstring);
    query.exec();

    while(query.next())
    {
        result.append(query.value(0).toString());
    }
    qDebug() << "db::RetreiveAircraftMake... Result:" << result;
    return result;
}
/*!
 * \brief dbAircraft::retreiveAircraftModel Search function to provide a
 * QStringList to the QCompleter
 * \param make A possible aircraft family (A320, 737,...)
 * \param searchstring
 * \return
 */
QStringList dbAircraft::retreiveAircraftModel(QString make, QString searchstring)
{
    QStringList result;
    QSqlQuery query;
    query.prepare("SELECT model FROM aircraft WHERE make = ? AND model LIKE ?");
    query.addBindValue(make);
    searchstring.prepend("%"); searchstring.append("%");
    query.addBindValue(searchstring);
    query.exec();

    while(query.next())
    {
        result.append(query.value(0).toString());
        qDebug() << "db::RetreiveAircraftModel... Result:" << result;
    }
    return result;
}
/*!
 * \brief dbAircraft::retreiveAircraftVariant Search function to provide a
 * QStringList to the QCompleter
 * \param make Aircraft manufacturer
 * \param model Aircraft family
 * \param searchstring
 * \return Aircraft Variant
 */
QStringList dbAircraft::retreiveAircraftVariant(QString make, QString model, QString searchstring)
{
    QStringList result;
    QSqlQuery query;
    query.prepare("SELECT variant from aircraft WHERE make = ? AND model = ? AND variant LIKE ?");
    query.addBindValue(make);
    query.addBindValue(model);
    searchstring.prepend("%"); searchstring.append("%");
    query.addBindValue(searchstring);
    query.exec();

    while(query.next())
    {
        result.append(query.value(0).toString());
        qDebug() << "db::RetreiveAircraftVariant... Result:" << result;
    }

    return result;
}
/*!
 * \brief dbAircraft::retreiveAircraftIdFromMakeModelVariant Looks up the unique
 * aircraft id for a given specification of make, model and variant
 * \param make Aircraft manufacturer (e.g. Boeing)
 * \param model Aircraft family (e.g. 737)
 * \param variant Aircraft variant (e.g. -800)
 * \return arcraft_id primary key of aircraft database
 */
QString dbAircraft::retreiveAircraftIdFromMakeModelVariant(QString make, QString model, QString variant)
{
    QString result;
    QSqlQuery query;
    query.prepare("SELECT aircraft_id FROM aircraft WHERE make = ? AND model = ? AND variant = ?");
    query.addBindValue(make);
    query.addBindValue(model);
    query.addBindValue(variant);
    query.exec();

    if(query.first())
    {
        result.append(query.value(0).toString());
        qDebug() << "db::RetreiveAircraftIdFromMakeModelVariant: Aircraft found! ID# " << result;
        return result;
    }else
    {
        result = result.left(result.length()-1);
        result.append("0");
        qDebug() << "db::RetreiveAircraftIdFromMakeModelVariant: ERROR - no AircraftId found.";
        return  result;
    }
}
/*!
 * \brief dbAircraft::commitTailToDb Creates a new entry in the tails database
 * \param registration
 * \param aircraft_id Primary key of aircraft database
 * \param company optional entry if a/c is associated with a certain company
 * \return true on success
 */
bool dbAircraft::commitTailToDb(QString registration, QString aircraft_id, QString company)
{
    QSqlQuery commit;
    commit.prepare("INSERT INTO tails (registration, aircraft_id, company) VALUES (?,?,?)");
    commit.addBindValue(registration);
    commit.addBindValue(aircraft_id);
    commit.addBindValue(company);
    commit.exec();

    if(commit.lastError().text().length() < 0){
        qWarning() << "db::CommitAircraftToDb:: SQL error:" << commit.lastError().text();
        return false;
    }else{
        return true;
    }
}

QVector<QString> dbAircraft::retreiveAircraftDetails(QString aircraft_id)
{
    QSqlQuery query;
    query.prepare("SELECT singlepilot, multipilot, singleengine, "
                  "multiengine, turboprop, jet, heavy "
                  "FROM aircraft "
                  "WHERE aircraft_id = ?");
    query.addBindValue(aircraft_id);
    query.exec();

    QVector<QString> result;
    while(query.next())
    {
        result.append(query.value(0).toString()); // Singlepilot
        result.append(query.value(1).toString()); // Multipilot
        result.append(query.value(2).toString()); // Singlengine
        result.append(query.value(3).toString()); // Multiengine
        result.append(query.value(4).toString()); // turboprop
        result.append(query.value(5).toString()); // jet
        result.append(query.value(6).toString()); // heavy
        qDebug() << "dbaircraft::retreiveAircraftDetails... Result:" << result;
    }
    return result;
}
