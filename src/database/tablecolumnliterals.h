#ifndef TABLECOLUMNLITERALS_H
#define TABLECOLUMNLITERALS_H

#include <QString>
#include <QStringLiteral>

/// [F]: These constants deviate slightly from const naming convention to reflect their sql column name.
/// In many places throughout the application, we have to programatically access or write data
/// to or from the database using column names as identifiers, doing something like this:
///
/// newData.insert("dept", ui->deptLocLineEdit->text());
/// newData.value("multipilot") // do stuff
///
///
/// Declaring the literals here avoids memory allocation at runtime for construction of temporary qstrings
/// like ("dept"). See https://doc.qt.io/qt-5/qstring.html#QStringLiteral
///

// Table names
static const auto DB_TABLE_FLIGHTS  = QStringLiteral("flights");
static const auto DB_TABLE_PILOTS   = QStringLiteral("pilots");
static const auto DB_TABLE_TAILS    = QStringLiteral("tails");
static const auto DB_TABLE_AIRCRAFT = QStringLiteral("aircraft");
static const auto DB_TABLE_AIRPORTS = QStringLiteral("airports");

// Flights table
static const auto DB_FLIGHTS_DOFT           = QStringLiteral("doft");
static const auto DB_FLIGHTS_DEPT           = QStringLiteral("dept");
static const auto DB_FLIGHTS_DEST           = QStringLiteral("dest");
static const auto DB_FLIGHTS_TOFB           = QStringLiteral("tofb");
static const auto DB_FLIGHTS_TONB           = QStringLiteral("tonb");
static const auto DB_FLIGHTS_PIC            = QStringLiteral("pic");
static const auto DB_FLIGHTS_ACFT           = QStringLiteral("acft");
static const auto DB_FLIGHTS_TBLK           = QStringLiteral("tblk");
static const auto DB_FLIGHTS_TSPSE          = QStringLiteral("tSPSE");
static const auto DB_FLIGHTS_SPME           = QStringLiteral("tSPME");
static const auto DB_FLIGHTS_TMP            = QStringLiteral("tMP");
static const auto DB_FLIGHTS_TNIGHT         = QStringLiteral("tNIGHT");
static const auto DB_FLIGHTS_TIFR           = QStringLiteral("tIFR");
static const auto DB_FLIGHTS_TPIC           = QStringLiteral("tPIC");
static const auto DB_FLIGHTS_TPICUS         = QStringLiteral("tPICUS");
static const auto DB_FLIGHTS_TSIC           = QStringLiteral("tSIC");
static const auto DB_FLIGHTS_TDUAL          = QStringLiteral("tDUAL");
static const auto DB_FLIGHTS_TFI            = QStringLiteral("tFI");
static const auto DB_FLIGHTS_TSIM           = QStringLiteral("tSIM");
static const auto DB_FLIGHTS_PILOTFLYING    = QStringLiteral("pilotFlying");
static const auto DB_FLIGHTS_TODAY          = QStringLiteral("toDay");
static const auto DB_FLIGHTS_TONIGHT        = QStringLiteral("toNight");
static const auto DB_FLIGHTS_LDGDAY         = QStringLiteral("ldgDay");
static const auto DB_FLIGHTS_LDGNIGHT       = QStringLiteral("ldgNight");
static const auto DB_FLIGHTS_AUTOLAND       = QStringLiteral("autoland");
static const auto DB_FLIGHTS_SECONDPILOT    = QStringLiteral("secondPilot");
static const auto DB_FLIGHTS_THIRDPILOT     = QStringLiteral("thirdPilot");
static const auto DB_FLIGHTS_APPROACHTYPE   = QStringLiteral("approachType");
static const auto DB_FLIGHTS_FLIGHTNUMBER   = QStringLiteral("flightNumber");
static const auto DB_FLIGHTS_REMARKS        = QStringLiteral("remarks");

// tails table

static const auto DB_TAILS_REGISTRATION   = QStringLiteral("registration");
static const auto DB_TAILS_COMPANY        = QStringLiteral("company");
static const auto DB_TAILS_MAKE           = QStringLiteral("make");
static const auto DB_TAILS_MODEL          = QStringLiteral("model");
static const auto DB_TAILS_VARIANT        = QStringLiteral("variant");
static const auto DB_TAILS_MULTIPILOT     = QStringLiteral("multipilot");
static const auto DB_TAILS_MULTIENGINE    = QStringLiteral("multiengine");
static const auto DB_TAILS_ENGINETYPE     = QStringLiteral("engineType");
static const auto DB_TAILS_WEIGHTCLASS    = QStringLiteral("weightClass");

// pilots table

static const auto DB_PILOTS_LASTNAME       = QStringLiteral("lastname");
static const auto DB_PILOTS_FIRSTNAME      = QStringLiteral("firstname");
static const auto DB_PILOTS_ALIAS          = QStringLiteral("alias");
static const auto DB_PILOTS_COMPANY        = QStringLiteral("company");
static const auto DB_PILOTS_EMPLOYEEID     = QStringLiteral("employeeid");
static const auto DB_PILOTS_PHONE          = QStringLiteral("phone");
static const auto DB_PILOTS_EMAIL          = QStringLiteral("email");

// all tables

static const auto DB_ROWID          = QStringLiteral("ROWID");
static const auto DB_NULL           = QStringLiteral("");
static const auto DB_NULL_hhmm      = QStringLiteral("00:00");
/*static const auto DB_               = QStringLiteral("");
static const auto DB_               = QStringLiteral("");
static const auto DB_               = QStringLiteral("");
static const auto DB_               = QStringLiteral("");
static const auto DB_               = QStringLiteral("");*/

#endif // TABLECOLUMNLITERALS_H
