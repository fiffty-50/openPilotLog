/*
 *openPilot Log - A FOSS Pilot Logbook Application
 *Copyright (C) 2020  Felix Turowsky
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "dbpilots.h"


/*!
 * \brief RetreivePilotNameFromID Looks up pilot ID in database
 * \param pilotID pilot_id in database
 * \return Pilot Name
 */
QString dbPilots::retreivePilotNameFromID(QString pilotID)
{
    QString pilotName("");
    if (pilotID == "1")
    {
        pilotName = "self";
        return pilotName;
    }
    QSqlQuery query;
    query.prepare("SELECT piclastname, picfirstname, alias FROM pilots WHERE pilot_id == ?");
    query.addBindValue(pilotID.toInt());
    query.exec();

    while (query.next()) {
        pilotName.append(query.value(0).toString());
        pilotName.append(", ");
        pilotName.append(query.value(1).toString());//.left(1));
    }

    if(pilotName.length() == 0)
    {
        qDebug() << ("No Pilot with this ID found");
    }
    return pilotName;
}

/*!
 * \brief dbPilots::retreivePilotIdFromString Looks up a pilot in the database
 * \param lastname pic_lastname in pilots table
 * \param firstname pic_firstname in pilots table
 * \return pilot_id from database or empty string.
 */
QString dbPilots::retreivePilotIdFromString(QString lastname, QString firstname)
{
    QSqlQuery query;
    query.prepare("SELECT pilot_id from pilots "
                  "WHERE piclastname = ? AND picfirstname LIKE ?");
    query.addBindValue(lastname);
    firstname.prepend("%"); firstname.append("%");
    query.addBindValue(firstname);
    query.exec();

    QString id;
    if(query.first()){id.append(query.value(0).toString());}

    return id;
}

/*!
 * \brief dbPilots::retreivePilotNameFromString Searches the pilot Name
 * in the Database and returns the name as a vector of results unless the pilot in command is the logbook owner.
 * \param searchstring
 * \return
 */
QStringList dbPilots::retreivePilotNameFromString(QString searchstring)
{
    QString firstname = searchstring; //To Do: Two control paths, one for single word, query as before with only searchstring
    QString lastname = searchstring;  // second control path with comma, lastname like AND firstname like
    if(searchstring.contains(QLatin1Char(',')))
    {
        QStringList namelist = searchstring.split(QLatin1Char(','));
        QString lastname = namelist[0].trimmed();
        lastname = lastname.toLower();
        lastname[0] = lastname[0].toUpper();
        lastname.prepend("%"), lastname.append("%");

        QString firstname = namelist[1].trimmed();
        if(firstname.length()>1)
        {
            firstname = firstname.toLower();
            firstname[0] = firstname[0].toUpper();
            firstname.prepend("%"), firstname.append("%");
        }
        qDebug() << "db::RetreivePilotNameFromString: first last after comma";
        qDebug() << firstname << lastname;
    }
    QSqlQuery query;
    query.prepare("SELECT piclastname, picfirstname, alias "
                  "FROM pilots WHERE  "
                  "picfirstname LIKE ? OR piclastname LIKE ? OR alias LIKE ?");
    searchstring.prepend("%");
    searchstring.append("%");
    query.addBindValue(firstname);
    query.addBindValue(lastname);
    query.addBindValue(searchstring);
    query.exec();

    QStringList result;
    while (query.next()) {
        QString piclastname = query.value(0).toString();
        QString picfirstname = query.value(1).toString();
        QString alias = query.value(2).toString();
        QString name = piclastname + ", " + picfirstname;
        result.append(name);
    }
    qDebug() << "db::RetreivePilotNameFromString Result: "  << result;

    if(result.size() == 0)
    {
        qDebug() << ("db::RetreivePilotNameFromString: No Pilot found");
        return result;
    }
    return result;
}

/*!
 * \brief newPicGetString This function is returning a QStringList for the QCompleter in the NewFlight::newPic line edit
 * A regular expression limits the input possibilities to only characters,
 * followed by an optional ',' and 1 whitespace, e.g.:
 * Miller, Jim ->valid / Miller,  Jim -> invalid / Miller,, Jim -> invalid
 * Miller Jim -> valid / Miller  Jim ->invalid
 * Jim Miller-> valid
 * \param searchstring
 * \return
 */
QStringList dbPilots::newPicGetString(QString searchstring)
{
    qWarning() << "newPicGetString is deprecated";
    QStringList result;
    QStringList searchlist;

    if(searchstring == "self")
    {
        result.append("self");
        qDebug() << "Pilot is self";
        return result;
    }

    //Case 1) Lastname, Firstname
    if(searchstring.contains(QLatin1Char(',')))
    {
        QStringList namelist = searchstring.split(QLatin1Char(','));

        QString name1 = namelist[0].trimmed();
        name1 = name1.toLower();
        name1[0] = name1[0].toUpper();
        searchlist.append(name1);

        if(namelist[1].length() > 1)
        {
            QString name2 = namelist[1].trimmed();
            name2 = name2.toLower();
            name2[0] = name2[0].toUpper();
            searchlist.append(name2);
        }
    }
    //Case 2: Firstname Lastname
    if(searchstring.contains(" ") && !searchstring.contains(QLatin1Char(',')))
    {
        QStringList namelist = searchstring.split(" ");

        QString name1 = namelist[0].trimmed();
        name1 = name1.toLower();
        name1[0] = name1[0].toUpper();
        searchlist.append(name1);

        if(namelist[1].length() > 1) //To avoid index out of range if the searchstring is one word followed by only one whitespace
        {
            QString name2 = namelist[1].trimmed();
            name2 = name2.toLower();
            name2[0] = name2[0].toUpper();
            searchlist.append(name2);
        }
    }
    //Case 3: Lastname
    if(!searchstring.contains(" ") && !searchstring.contains(QLatin1Char(',')))
    {

        QString name1 = searchstring.toLower();
        name1[0] = name1[0].toUpper();
        searchlist.append(name1);
    }

    if(searchlist.length() == 1)
    {
        QSqlQuery query;
        query.prepare("SELECT piclastname, picfirstname FROM pilots "
                      "WHERE piclastname LIKE ?");
        query.addBindValue(searchlist[0] + '%');
        query.exec();



        while(query.next())
        {
            result.append(query.value(0).toString() + ", " + query.value(1).toString());
        }

        QSqlQuery query2;
        query2.prepare("SELECT piclastname, picfirstname FROM pilots "
                       "WHERE picfirstname LIKE ?");
        query2.addBindValue(searchlist[0] + '%');
        query2.exec();

        while(query2.next())
        {
            result.append(query2.value(0).toString() + ", " + query2.value(1).toString());
        }
    }else
    {
        QSqlQuery query;
        query.prepare("SELECT piclastname, picfirstname FROM pilots "
                      "WHERE piclastname LIKE ? AND picfirstname LIKE ?");
        query.addBindValue(searchlist[0] + '%');
        query.addBindValue(searchlist[1] + '%');
        query.exec();

        while(query.next())
        {
            result.append(query.value(0).toString() + ", " + query.value(1).toString());
        }

        QSqlQuery query2;
        query2.prepare("SELECT piclastname, picfirstname FROM pilots "
                       "WHERE picfirstname LIKE ? AND piclastname LIKE ?");
        query2.addBindValue(searchlist[0] + '%');
        query2.addBindValue(searchlist[1] + '%');
        query2.exec();

        while(query2.next())
        {
            result.append(query2.value(0).toString() + ", " + query2.value(1).toString());
        }
    }
    qDebug() << "db::newPic Result" << result.length() << result;
    if(result.length() == 0)
    {
        //To Do: try first name search
        qDebug() << "No Pilot with this name found";
        return result;
    }else
    {
        return result;
    }
}

QStringList dbPilots::retreivePilotList()
{
    QSqlQuery query;
    query.prepare("SELECT piclastname, picfirstname FROM pilots");
    query.exec();

    QStringList result;
    while (query.next()) {
        result.append(query.value(0).toString() + ", " + query.value(1).toString());
    }
    return result;
}

QString dbPilots::newPicGetId(QString name)
{
    QString result;

    QStringList nameparts = name.split(QLatin1Char(','));
    QString lastname = nameparts[0].trimmed();
    lastname = lastname.toLower(); lastname[0] = lastname[0].toUpper();
    QString firstname = nameparts[1].trimmed();
    firstname = firstname.toLower(); firstname[0] = firstname[0].toUpper();
    firstname.prepend("%"); firstname.append("%");

    QSqlQuery query;
    query.prepare("SELECT pilot_id FROM pilots "
                  "WHERE piclastname = ? AND picfirstname LIKE ?");
    query.addBindValue(lastname);
    query.addBindValue(firstname);
    query.exec();

    while (query.next())
    {
        result.append(query.value(0).toString());
    }

    qDebug() << "newPicGetId: result = " << result;
    return result;
}
