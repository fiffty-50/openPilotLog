#ifndef DECLARATIONS_H
#define DECLARATIONS_H

#include <QtCore>

/*!
 * \brief An alias for QString
 *
 * Very long description *with* **markdown?**
 * - - -
 * # Header
 */
using ColName = QString;
using ColData = QVariant;
using TableName = QString;
using RowId = int;

using TableNames = QStringList;
// [G]: May lead to some confusion. TableData suggest data for the entire table.
// but in reallity it is data per column *of single row* (unless i misunderstand)
// [F]: That's correct. We could maybe call it EntryData or RowData?
using RowData = QMap<ColName, ColData>;
using ColumnData = QPair<ColName, ColData>;
using ColumnNames = QStringList;
using TableColumns = QMap<TableName, ColumnNames>;

// [G]: Needs some work. Inheriting from QPair may be helpful but
// may also be overkill. Lets determine the specific uses of DataPosition
// and provide our own interface i would say.
// [F]: Good idea! Implementing something similar to first and second methods
// of QPair would be useful to carry over, or some other way of quickly and
// unambiguously accessing the elements.
struct DataPosition : QPair<TableName, RowId> {
    TableName tableName;
    RowId rowId;
    DataPosition()
        : tableName(first), rowId(second)
    {}
    DataPosition(TableName table_name, RowId row_id)
        : QPair<TableName, RowId>::QPair(table_name, row_id),
          tableName(first), rowId(second)
    {}
    DataPosition(const DataPosition& other) = default;
    DataPosition& operator=(const DataPosition& other) = default;
};

// [F]:
// In many places throughout the application, we have to programatically access or write data
// to or from the database using column names as identifiers, doing something like this:
//
// newData.insert("dept", ui->deptLocLineEdit->text());
// newData.value("multipilot") // do stuff
//
// Declaring the literals here avoids memory allocation at runtime for construction of temporary qstrings
// like ("dept"). See https://doc.qt.io/qt-5/qstring.html#QStringLiteral
//

// Table names
static const auto DB_TABLE_FLIGHTS  = QStringLiteral("flights");
static const auto DB_TABLE_PILOTS   = QStringLiteral("pilots");
static const auto DB_TABLE_TAILS    = QStringLiteral("tails");
static const auto DB_TABLE_AIRCRAFT = QStringLiteral("aircraft");
static const auto DB_TABLE_AIRPORTS = QStringLiteral("airports");
// Default Positions
static auto const DEFAULT_FLIGHT_POSITION   = DataPosition(DB_TABLE_FLIGHTS, 0);
static auto const DEFAULT_PILOT_POSITION    = DataPosition(DB_TABLE_PILOTS, 0);
static auto const DEFAULT_TAIL_POSITION     = DataPosition(DB_TABLE_TAILS, 0);
static auto const DEFAULT_AIRCRAFT_POSITION = DataPosition(DB_TABLE_AIRCRAFT, 0);
// Flights table columns
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


#endif // DECLARATIONS_H
