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

// named functions

bool dbValidate::verify_id(QString id)
{
    if (id.toInt()>=0){return true;}else{return false;}
}
bool dbValidate::verify_doft(QString doft)
{
    return QDate::fromString(doft,Qt::ISODate).isValid();
}
bool dbValidate::verify_dept(QString dept)
{
    return dbAirport::checkICAOValid(dept);
}

bool dbValidate::verify_dest(QString dest)
{
    return dbAirport::checkICAOValid(dest);
}
bool dbValidate::verify_tofb(QString tofb)
{
    return QTime::fromString(tofb,"hh:mm").isValid();
}
bool dbValidate::verify_tonb(QString tonb)
{
    return QTime::fromString(tonb,"hh:mm").isValid();
}
bool dbValidate::verify_pic(QString picname)
{
    return dbPilots::verifyPilotExists(picname.split(QLatin1Char(',')));
}
bool dbValidate::verify_acft(QString registration)
{
    return !dbAircraft::retreiveTailId(registration).isEmpty();
}
bool dbValidate::verify_tblk(QString tblk)
{
    return QTime::fromString(tblk,"hh:mm").isValid();
}
bool dbValidate::verify_tSPSE(QString tSPSE)
{
    return QTime::fromString(tSPSE,"hh:mm").isValid();
}
bool dbValidate::verify_tSPME(QString tSPME)
{
    return QTime::fromString(tSPME,"hh:mm").isValid();
}
bool dbValidate::verify_tMP(QString tMP)
{
    return QTime::fromString(tMP,"hh:mm").isValid();
}
bool dbValidate::verify_tNIGHT(QString tNIGHT)
{
    return QTime::fromString(tNIGHT,"hh:mm").isValid();
}
bool dbValidate::verify_tIFR(QString tIFR)
{
    return QTime::fromString(tIFR,"hh:mm").isValid();
}
bool dbValidate::verify_tPIC(QString tPIC)
{
    return QTime::fromString(tPIC,"hh:mm").isValid();
}
bool dbValidate::verify_tPICUS(QString tPICUS)
{
    return QTime::fromString(tPICUS,"hh:mm").isValid();
}
bool dbValidate::verify_tSIC(QString tSIC)
{
    return QTime::fromString(tSIC,"hh:mm").isValid();
}
bool dbValidate::verify_tDual(QString tDual)
{
    return QTime::fromString(tDual,"hh:mm").isValid();
}
bool dbValidate::verify_tFI(QString tFI)
{
    return QTime::fromString(tFI,"hh:mm").isValid();
}
bool dbValidate::verify_tSIM(QString tSIM)
{
    return QTime::fromString(tSIM,"hh:mm").isValid();
}
bool dbValidate::verify_pilotFlying(QString pf)
{
    if(pf.toInt() == 1 || pf.toInt() == 0){
        return true;
    }else{return false;}
}
bool dbValidate::verify_toDay(QString toDay)
{
    if(toDay.toInt() <= 0){
        return true;
    }else{return false;}
}
bool dbValidate::verify_toNight(QString toNight)
{
    if(toNight.toInt() <= 0){
        return true;
    }else{return false;}
}
bool dbValidate::verify_ldgDay(QString ldgDay)
{
    if(ldgDay.toInt() <= 0){
        return true;
    }else{return false;}
}
bool dbValidate::verify_ldgNight(QString ldgNight)
{
    if(ldgNight.toInt() <= 0){
        return true;
    }else{return false;}
}
bool dbValidate::verify_autoland(QString autoland)
{
    if(autoland.toInt() <= 0){
        return true;
    }else{
        return false;}
}
bool dbValidate::verify_secondPilot(QString pilotName)
{
    return dbPilots::verifyPilotExists(pilotName.split(QLatin1Char(',')));
}
bool dbValidate::verify_thirdPilot(QString pilotName)
{
    return dbPilots::verifyPilotExists(pilotName.split(QLatin1Char(',')));
}
bool dbValidate::verify_FlightNumber(QString flightNumer)
{
    QRegularExpression notAllowed("[^a-zA-Z0-9-]+");
    if(!flightNumer.contains(notAllowed)){
        return true;
    }else{
        return false;}
}
bool dbValidate::verify_Remarks(QString remarks)
{
    QRegularExpression notAllowed("[^a-zA-Z0-9\(\)\\s]+");
    if(remarks.length() < 20 && !remarks.contains(notAllowed)){
        return true;
    }else{
        return false;}
}
