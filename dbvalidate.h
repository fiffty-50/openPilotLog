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
#ifndef DBVALIDATE_H
#define DBVALIDATE_H
#include <QCoreApplication>

/*!
 * \brief The dbValidate class provides functions providing validation functionality
 * for data entries. All return types are boolean (true = valid, false = invalid)
 */
class dbValidate
{
public:

    bool isValid;

    dbValidate();

    dbValidate(QString table, QString field, QString value);

    bool verify_id(QString);
    bool verify_doft(QString);
    bool verify_dept(QString);
    bool verify_dest(QString);
    bool verify_tofb(QString);
    bool verify_tonb(QString);
    bool verify_pic(QString);
    bool verify_acft(QString);
    bool verify_tblk(QString);
    bool verify_tSPSE(QString);
    bool verify_tSPME(QString);
    bool verify_tMP(QString);
    bool verify_tNIGHT(QString);
    bool verify_tIFR(QString);
    bool verify_tPIC(QString);
    bool verify_tPICUS(QString);
    bool verify_tSIC(QString);
    bool verify_tDual(QString);
    bool verify_tFI(QString);
    bool verify_tSIM(QString);
    bool verify_pilotFlying(QString);
    bool verify_toDay(QString);
    bool verify_toNight(QString);
    bool verify_ldgDay(QString);
    bool verify_ldgNight(QString);
    bool verify_autoland(QString);
    bool verify_secondPilot(QString);
    bool verify_thirdPilot(QString);
    bool verify_FlightNumber(QString);
    bool verify_Remarks(QString);
};

#endif // DBVALIDATE_H
