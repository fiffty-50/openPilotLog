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
#include "dbvalidate.h"
#include "dbapi.h"



dbValidate::dbValidate()
{
    isValid = false;
    qDebug() << "Default Constructor";
}



bool dbValidate::verify_id(QString id)
{
    if (id.toInt()>=0){return true;}else{return false;}
}
bool verify_doft(QString doft)
{
    return QDate::fromString(doft,Qt::ISODate).isValid();
}
bool verify_dept(QString dept)
{
    return dbAirport::checkICAOValid(dept);
}

bool verify_dest(QString dest)
{
    return dbAirport::checkICAOValid(dest);
}
bool verify_tofb(QString tofb)
{
    return QTime::fromString(tofb,"hh:mm").isValid();
}
bool verify_tonb(QString tonb)
{
    return QTime::fromString(tonb,"hh:mm").isValid();
}
bool verify_pic(QString picname)
{
    return dbPilots::verifyPilotExists(picname.split(QLatin1Char(',')));
}
bool verify_acft(QString registration)
{
    return !dbAircraft::retreiveTailId(registration).isEmpty();
}
bool verify_tblk(QString tblk)
{
    return QTime::fromString(tblk,"hh:mm").isValid();
}
/*bool verify_tSPSE();
bool verify_tSPME();
bool verify_tMP();
bool verify_tNIGHT();
bool verify_tIFR();
bool verify_tPIC();
bool verify_tPICUS();
bool verify_tSIC();
bool verify_tDual();
bool verify_tFI();
bool verify_tSIM();
bool verify_pilotFlying();
bool verify_toDay();
bool verify_toNight();
bool verify_ldgDay();
bool verify_ldgNight();
bool verify_autoland();
bool verify_secondPilot();
bool verify_thirdPilot();
bool verify_FlightNumber();
bool verify_Remarks();*/
