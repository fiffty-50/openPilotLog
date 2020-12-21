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
static const auto DB_doft           = QStringLiteral("doft");
static const auto DB_dept           = QStringLiteral("dept");
static const auto DB_dest           = QStringLiteral("dest");
static const auto DB_tofb           = QStringLiteral("tofb");
static const auto DB_tonb           = QStringLiteral("tonb");
static const auto DB_pic            = QStringLiteral("pic");
static const auto DB_acft           = QStringLiteral("acft");
static const auto DB_tblk           = QStringLiteral("tblk");
static const auto DB_tSPSE          = QStringLiteral("tSPSE");
static const auto DB_tSPME          = QStringLiteral("tSPME");
static const auto DB_tMP            = QStringLiteral("tMP");
static const auto DB_tNIGHT         = QStringLiteral("tNIGHT");
static const auto DB_tIFR           = QStringLiteral("tIFR");
static const auto DB_tPIC           = QStringLiteral("tPIC");
static const auto DB_tPICUS         = QStringLiteral("tPICUS");
static const auto DB_tSIC           = QStringLiteral("tSIC");
static const auto DB_tDUAL          = QStringLiteral("tDUAL");
static const auto DB_tFI            = QStringLiteral("tFI");
static const auto DB_tSIM           = QStringLiteral("tSIM");
static const auto DB_pilotFlying    = QStringLiteral("pilotFlying");
static const auto DB_toDay          = QStringLiteral("toDay");
static const auto DB_toNight        = QStringLiteral("toNight");
static const auto DB_ldgDay         = QStringLiteral("ldgDay");
static const auto DB_ldgNight       = QStringLiteral("ldgNight");
static const auto DB_autoland       = QStringLiteral("autoland");
static const auto DB_secondPilot    = QStringLiteral("secondPilot");
static const auto DB_thirdPilot     = QStringLiteral("thirdPilot");
static const auto DB_ApproachType   = QStringLiteral("ApproachType");
static const auto DB_FlightNumber   = QStringLiteral("FlightNumber");
static const auto DB_Remarks        = QStringLiteral("Remarks");

// aircraft and tails tables

static const auto DB_registration   = QStringLiteral("registration");
//static const auto DB_company      = QStringLiteral("company"); to do - resolve naming conflict with pilots.company, time being these are equivalent since they are unique in their respective tables but still..
static const auto DB_make           = QStringLiteral("make");
static const auto DB_model          = QStringLiteral("model");
static const auto DB_variant        = QStringLiteral("variant");
static const auto DB_multipilot     = QStringLiteral("multipilot");
static const auto DB_multiengine    = QStringLiteral("multiengine");
static const auto DB_engineType     = QStringLiteral("engineType");
static const auto DB_weightClass    = QStringLiteral("weightClass");

// pilots table

static const auto DB_piclastname    = QStringLiteral("piclastname");
static const auto DB_picfirstname   = QStringLiteral("picfirstname");
static const auto DB_alias          = QStringLiteral("alias");
static const auto DB_company        = QStringLiteral("company");
static const auto DB_employeeid     = QStringLiteral("employeeid");
static const auto DB_phone          = QStringLiteral("phone");
static const auto DB_email          = QStringLiteral("email");

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
