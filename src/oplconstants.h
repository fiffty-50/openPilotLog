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
#ifndef OPLCONSTANTS_H
#define OPLCONSTANTS_H

#include <QtCore>

/*!
 *  \brief A namespace to collect constants and enums used throughout the application.
 *
 *  \details The opl namespace collects enums and constants that are used throughout
 *  the application and provide uniform access.
 *
 *  The date, time and datetime namespaces include enums used to differentiate
 *  date and time formats for QDate, QTime and QDateTime that deviate from standard values
 *  included in the Qt Framework like Qt::ISODate and are to be used in conjunction with the
 *  .toString() members of these classes.
 *
 *  The db namespace contains constants for programatically accessing the database in a fast
 *  and uniform manner.
 */
namespace Opl {

static const auto ApproachTypes = QStringList{
        QStringLiteral("VISUAL"),
        QStringLiteral("ILS CAT I"),
        QStringLiteral("ILS CAT II"),
        QStringLiteral("ILS CAT III"),
        QStringLiteral("GLS"),
        QStringLiteral("MLS"),
        QStringLiteral("LOC"),
        QStringLiteral("LOC/DME"),
        QStringLiteral("RNAV"),
        QStringLiteral("RNAV (LNAV)"),
        QStringLiteral("RNAV (LNAV/VNAV)"),
        QStringLiteral("RNAV (LPV)"),
        QStringLiteral("RNAV (RNP)"),
        QStringLiteral("RNAV (RNP-AR)"),
        QStringLiteral("VOR"),
        QStringLiteral("VOR/DME"),
        QStringLiteral("NDB"),
        QStringLiteral("NDB/DME"),
        QStringLiteral("TACAN"),
        QStringLiteral("SRA"),
        QStringLiteral("PAR"),
        QStringLiteral("OTHER")
};

namespace Date {

enum DateFormat {Default, Text};

} // namespace opl::date

namespace Time {

enum FlightTimeFormat {Default, Decimal};

} // namespace opl::time

namespace Datetime {

enum DateTimeFormat {Default, Backup};

} // namespace opl::datetime

/*!
 *  The opl::db namespace provides string literals to programatically access the database
 *
 *  Example usage, do:
 *  newData.insert(opl::db::FLIGHTS_DEP, ui->deptLocLineEdit->text());
 *  newData.value(opl::db::AIRCRAFT_MULTIPILOT);
 *
 *  instead of:
 *  newData.insert("dept", ui->deptLocLineEdit->text());
 *  newData.value("multipilot");
 *
 *  Declaring these literals here avoids memory allocation at runtime for construction of temporary
 *  qstrings like ("dept"). See https://doc.qt.io/qt-5/qstring.html#QStringLiteral and ensures
 *  uniform use throughout the application.
 */
namespace Db {


// Table names
static const auto TABLE_FLIGHTS          = QStringLiteral("flights");
static const auto TABLE_PILOTS           = QStringLiteral("pilots");
static const auto TABLE_TAILS            = QStringLiteral("tails");
static const auto TABLE_AIRCRAFT         = QStringLiteral("aircraft");
static const auto TABLE_AIRPORTS         = QStringLiteral("airports");

// Flights table columns
static const auto FLIGHTS_DOFT           = QStringLiteral("doft");
static const auto FLIGHTS_DEPT           = QStringLiteral("dept");
static const auto FLIGHTS_DEST           = QStringLiteral("dest");
static const auto FLIGHTS_TOFB           = QStringLiteral("tofb");
static const auto FLIGHTS_TONB           = QStringLiteral("tonb");
static const auto FLIGHTS_PIC            = QStringLiteral("pic");
static const auto FLIGHTS_ACFT           = QStringLiteral("acft");
static const auto FLIGHTS_TBLK           = QStringLiteral("tblk");
static const auto FLIGHTS_TSPSE          = QStringLiteral("tSPSE");
static const auto FLIGHTS_TSPME          = QStringLiteral("tSPME");
static const auto FLIGHTS_TMP            = QStringLiteral("tMP");
static const auto FLIGHTS_TNIGHT         = QStringLiteral("tNIGHT");
static const auto FLIGHTS_TIFR           = QStringLiteral("tIFR");
static const auto FLIGHTS_TPIC           = QStringLiteral("tPIC");
static const auto FLIGHTS_TPICUS         = QStringLiteral("tPICUS");
static const auto FLIGHTS_TSIC           = QStringLiteral("tSIC");
static const auto FLIGHTS_TDUAL          = QStringLiteral("tDUAL");
static const auto FLIGHTS_TFI            = QStringLiteral("tFI");
static const auto FLIGHTS_TSIM           = QStringLiteral("tSIM");
static const auto FLIGHTS_PILOTFLYING    = QStringLiteral("pilotFlying");
static const auto FLIGHTS_TODAY          = QStringLiteral("toDay");
static const auto FLIGHTS_TONIGHT        = QStringLiteral("toNight");
static const auto FLIGHTS_LDGDAY         = QStringLiteral("ldgDay");
static const auto FLIGHTS_LDGNIGHT       = QStringLiteral("ldgNight");
static const auto FLIGHTS_AUTOLAND       = QStringLiteral("autoland");
static const auto FLIGHTS_SECONDPILOT    = QStringLiteral("secondPilot");
static const auto FLIGHTS_THIRDPILOT     = QStringLiteral("thirdPilot");
static const auto FLIGHTS_APPROACHTYPE   = QStringLiteral("approachType");
static const auto FLIGHTS_FLIGHTNUMBER   = QStringLiteral("flightNumber");
static const auto FLIGHTS_REMARKS        = QStringLiteral("remarks");

// tails table

static const auto TAILS_REGISTRATION     = QStringLiteral("registration");
static const auto TAILS_COMPANY          = QStringLiteral("company");
static const auto TAILS_MAKE             = QStringLiteral("make");
static const auto TAILS_MODEL            = QStringLiteral("model");
static const auto TAILS_VARIANT          = QStringLiteral("variant");
static const auto TAILS_MULTIPILOT       = QStringLiteral("multipilot");
static const auto TAILS_MULTIENGINE      = QStringLiteral("multiengine");
static const auto TAILS_ENGINETYPE       = QStringLiteral("engineType");
static const auto TAILS_WEIGHTCLASS      = QStringLiteral("weightClass");

// pilots table

static const auto PILOTS_LASTNAME        = QStringLiteral("lastname");
static const auto PILOTS_FIRSTNAME       = QStringLiteral("firstname");
static const auto PILOTS_ALIAS           = QStringLiteral("alias");
static const auto PILOTS_COMPANY         = QStringLiteral("company");
static const auto PILOTS_EMPLOYEEID      = QStringLiteral("employeeid");
static const auto PILOTS_PHONE           = QStringLiteral("phone");
static const auto PILOTS_EMAIL           = QStringLiteral("email");

// all tables

static const auto ROWID                  = QStringLiteral("ROWID");
static const auto EMPTY_STRING           = QStringLiteral("");
static const auto NULL_TIME_hhmm         = QStringLiteral("00:00");

} // namespace opl::db

} // namespace opl

#endif // OPLCONSTANTS_H