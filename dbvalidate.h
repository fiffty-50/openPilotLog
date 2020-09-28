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
#include <QRegularExpression>

/*!
 * \brief The dbValidate class provides functions providing validation functionality
 * for data entries. All return types are boolean (true = valid, false = invalid) and
 * all input types are QString.
 */
class dbValidate
{
public:

    bool isValid;

    //dbValidate();

    //dbValidate(QString table, QString field, QString value);


    static bool verify_id(QString);
    static bool verify_doft(QString);
    static bool verify_dept(QString);
    static bool verify_dest(QString);
    static bool verify_tofb(QString);
    static bool verify_tonb(QString);
    static bool verify_pic(QString);
    static bool verify_acft(QString);
    static bool verify_tblk(QString);
    static bool verify_tSPSE(QString);
    static bool verify_tSPME(QString);
    static bool verify_tMP(QString);
    static bool verify_tNIGHT(QString);
    static bool verify_tIFR(QString);
    static bool verify_tPIC(QString);
    static bool verify_tPICUS(QString);
    static bool verify_tSIC(QString);
    static bool verify_tDual(QString);
    static bool verify_tFI(QString);
    static bool verify_tSIM(QString);
    static bool verify_pilotFlying(QString);
    static bool verify_toDay(QString);
    static bool verify_toNight(QString);
    static bool verify_ldgDay(QString);
    static bool verify_ldgNight(QString);
    static bool verify_autoland(QString);
    static bool verify_secondPilot(QString);
    static bool verify_thirdPilot(QString);
    static bool verify_FlightNumber(QString);
    static bool verify_Remarks(QString);
    static bool verify_time(QString time);
};

#endif // DBVALIDATE_H
