#include "flight.h"

/*!
 * \brief flight::printFlight Displays basic data for debugging
 */
void flight::printFlight()
{
    QTextStream cout(stdout, QIODevice::WriteOnly);

    if(id != -1){
        cout << "Flight ID:\t\t" + QString::number(id) + "\n";
    }else{
        cout << "Flight ID:\t\tnot set\n";
    }

    if(doft.toString(Qt::ISODate).length()){
        cout << "Date of Flight:\t" + doft.toString(Qt::ISODate) + "\n";
    }else{
        cout << "Date of Flight:\tnot set\n";
    }

   if(dept != QStringLiteral("INVA")){
       cout << "Departure:\t\t" + dept  + "\n";
    }else{
       cout << "Departure:\t\tnot set\n";
    }

   if(dest != QStringLiteral("INVA")){
       cout << "Destination:\t\t" + dest  + "\n";
    }else{
       cout << "Destination:\t\tnot set\n";
    }

   if(tofb.toString("hh:mm").length()){
       cout << "Departure Time:\t" + tofb.toString("hh:mm") + "\n";
   }else{
       cout << "Departure Time:\tnot set\n";
   }

   if(tonb.toString("hh:mm").length()){
       cout << "Arrival Time:\t" + tonb.toString("hh:mm") + "\n";
   }else{
       cout << "Arrival Time:\tnot set\n";
   }

    if(pic != QStringLiteral("INVA")){
        cout << "Pilot in Command:\t" + pic + "\n";
    }else{
       cout << "Pilot in Command:\tnot set\n";
    }

    if(acft != QStringLiteral("INVA")){
        cout << "Aircraft:\t\t" + acft + "\n";
    }else{
       cout << "Aircraft:\t\tnot set\n";
    }

    if(tblk.isValid()){
        cout << "Blocktime:\t\t" + tblk.toString("hh:mm") + "\n";
    }else{
       cout << "Blocktime:\t\tnot set\n";
    }
}
/*!
 * \brief flight::debug Provides compatibility with qDebug
 * \return
 */
QString flight::debug()
{
    printFlight();
    return QString();
}

flight flight::fromVector(QVector<QString> details)
{
    if(details.length() != 31){
        qWarning() << __PRETTY_FUNCTION__ << "Invalid Input. Aborting.";
        return flight();
    }
    flight flightobject;
    flightobject.id      = details[1].toInt();
    flightobject.doft    = QDate::fromString(details[2],Qt::ISODate);
    flightobject.dept    = details[3];
    flightobject.dest    = details[4];
    flightobject.tofb    = QTime::fromString(details[5],"hh:mm");
    flightobject.tonb    = QTime::fromString(details[6],"hh:mm");
    flightobject.pic     = details[7];
    flightobject.acft    = details[8];
    flightobject.tblk    = QTime::fromString(details[9],"hh:mm");
    flightobject.tSPSE = details[10].toInt();
    flightobject.tSPME = details[11].toInt();
    flightobject.tMP = details[12].toInt();
    flightobject.tNIGHT = details[13].toInt();
    flightobject.tIFR = details[14].toInt();

    flightobject.tPIC = details[15].toInt();
    flightobject.tSIC = details[16].toInt();
    flightobject.tDUAL = details[17].toInt();
    flightobject.tFI = details[18].toInt();

    flightobject.tSIM = details[19].toInt();

    flightobject.pilotFlying = details[20].toInt();
    flightobject.toDay = details[21].toInt();
    flightobject.toNight = details[22].toInt();
    flightobject.ldgDay = details[23].toInt();
    flightobject.ldgNight = details[24].toInt();
    flightobject.autoland = details[25].toInt();

    flightobject.secondPilot = details[26];
    flightobject.thirdPilot = details[27];
    flightobject.approachType = details[28];
    flightobject.flightNumber = details[29];
    flightobject.remarks = details[30];

    return flightobject;
}
