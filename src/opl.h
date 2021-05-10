/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2021 Felix Turowsky
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
#include <QMessageBox>
#include "src/database/adatabasetypes.h"

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

#if defined(__GNUC__) || defined(__clang__)
    #define FUNC_IDENT __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define FUNC_IDENT __FUNCSIG__
#else
    #define FUNC_IDENT __func__
#endif

#define DEB qDebug()                            // Use for debugging
#define LOG qInfo()                             // Use for logging milestones (silently, will be written to log file and console out only)
#define TODO qCritical() << "TO DO:\t"

#define INFO(msg) Opl::ANotificationHandler::info(msg, this)  // Use for messages of interest to the user (will be displayed in GUI)
#define WARN(msg) Opl::ANotificationHandler::warn(msg, this)  // Use for warnings (will be displayed in GUI)
#define CRIT(msg) Opl::ANotificationHandler::crit(msg, this)  // Use for critical warnings (will be displayed in GUI)

/*!
 * \brief The ANotificationHandler class handles displaying of user-directed messages. It displays
 * information to the user and forwards the displayed message to ALog so it is written
 * to the console and log files. The INFO, WARN and CRIT makros provide convenient access.
 */
class ANotificationHandler {
public:
    static inline void info(const QString msg, QWidget *parent = nullptr){
        qInfo() << msg;
        QMessageBox mb(parent);
        mb.setText(msg);
        mb.setIcon(QMessageBox::Information);
        mb.exec();
    };
    static inline void warn(const QString msg, QWidget *parent = nullptr){
        qWarning() << msg;
        QMessageBox mb(parent);
        mb.setText(msg);
        mb.setIcon(QMessageBox::Warning);
        mb.exec();
    };
    static inline void crit(const QString msg, QWidget *parent = nullptr){
        qCritical() << msg;
        QMessageBox mb(parent);
        mb.setText(msg);
        mb.setIcon(QMessageBox::Critical);
        mb.exec();
    };
}; // class ANotificationHandler

enum class Translations {English, German, Spanish};

const static QMap<Opl::Translations, QString> L10N_FILES {
    {Opl::Translations::English, QStringLiteral("l10n/openpilotlog_en")},
    {Opl::Translations::German,  QStringLiteral("l10n/openpilotlog_de")},
    {Opl::Translations::Spanish, QStringLiteral("l10n/openpilotlog_es")},
};
static const QMap<Translations, QString> L10N_NAMES {
    {Opl::Translations::English, QStringLiteral("English")},
    {Opl::Translations::German,  QStringLiteral("Deutsch")},
    {Opl::Translations::Spanish, QStringLiteral("Español")},
};

static const auto ApproachTypes = QStringList{
        QLatin1String("VISUAL"),
        QLatin1String("ILS CAT I"),
        QLatin1String("ILS CAT II"),
        QLatin1String("ILS CAT III"),
        QLatin1String("GLS"),
        QLatin1String("MLS"),
        QLatin1String("LOC"),
        QLatin1String("LOC/DME"),
        QLatin1String("RNAV"),
        QLatin1String("RNAV (LNAV)"),
        QLatin1String("RNAV (LNAV/VNAV)"),
        QLatin1String("RNAV (LPV)"),
        QLatin1String("RNAV (RNP)"),
        QLatin1String("RNAV (RNP-AR)"),
        QLatin1String("VOR"),
        QLatin1String("VOR/DME"),
        QLatin1String("NDB"),
        QLatin1String("NDB/DME"),
        QLatin1String("TACAN"),
        QLatin1String("SRA"),
        QLatin1String("PAR"),
        QLatin1String("OTHER")
};

namespace Date {

/*!
 * \brief ADateFormats enumerates the accepted date formats for QDateEdits
 */
enum class ADateFormat {ISODate, DE, EN };

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
static const auto TABLE_FLIGHTS          = QLatin1String("flights");
static const auto TABLE_PILOTS           = QLatin1String("pilots");
static const auto TABLE_TAILS            = QLatin1String("tails");
static const auto TABLE_AIRCRAFT         = QLatin1String("aircraft");
static const auto TABLE_AIRPORTS         = QLatin1String("airports");
static const auto TABLE_CURRENCIES       = QLatin1String("currencies");

// Flights table columns
static const auto FLIGHTS_DOFT           = QLatin1String("doft");
static const auto FLIGHTS_DEPT           = QLatin1String("dept");
static const auto FLIGHTS_DEST           = QLatin1String("dest");
static const auto FLIGHTS_TOFB           = QLatin1String("tofb");
static const auto FLIGHTS_TONB           = QLatin1String("tonb");
static const auto FLIGHTS_PIC            = QLatin1String("pic");
static const auto FLIGHTS_ACFT           = QLatin1String("acft");
static const auto FLIGHTS_TBLK           = QLatin1String("tblk");
static const auto FLIGHTS_TSPSE          = QLatin1String("tSPSE");
static const auto FLIGHTS_TSPME          = QLatin1String("tSPME");
static const auto FLIGHTS_TMP            = QLatin1String("tMP");
static const auto FLIGHTS_TNIGHT         = QLatin1String("tNIGHT");
static const auto FLIGHTS_TIFR           = QLatin1String("tIFR");
static const auto FLIGHTS_TPIC           = QLatin1String("tPIC");
static const auto FLIGHTS_TPICUS         = QLatin1String("tPICUS");
static const auto FLIGHTS_TSIC           = QLatin1String("tSIC");
static const auto FLIGHTS_TDUAL          = QLatin1String("tDUAL");
static const auto FLIGHTS_TFI            = QLatin1String("tFI");
static const auto FLIGHTS_TSIM           = QLatin1String("tSIM");
static const auto FLIGHTS_PILOTFLYING    = QLatin1String("pilotFlying");
static const auto FLIGHTS_TODAY          = QLatin1String("toDay");
static const auto FLIGHTS_TONIGHT        = QLatin1String("toNight");
static const auto FLIGHTS_LDGDAY         = QLatin1String("ldgDay");
static const auto FLIGHTS_LDGNIGHT       = QLatin1String("ldgNight");
static const auto FLIGHTS_AUTOLAND       = QLatin1String("autoland");
static const auto FLIGHTS_SECONDPILOT    = QLatin1String("secondPilot");
static const auto FLIGHTS_THIRDPILOT     = QLatin1String("thirdPilot");
static const auto FLIGHTS_APPROACHTYPE   = QLatin1String("approachType");
static const auto FLIGHTS_FLIGHTNUMBER   = QLatin1String("flightNumber");
static const auto FLIGHTS_REMARKS        = QLatin1String("remarks");

// tails table

static const auto TAILS_REGISTRATION     = QLatin1String("registration");
static const auto TAILS_COMPANY          = QLatin1String("company");
static const auto TAILS_MAKE             = QLatin1String("make");
static const auto TAILS_MODEL            = QLatin1String("model");
static const auto TAILS_VARIANT          = QLatin1String("variant");
static const auto TAILS_MULTIPILOT       = QLatin1String("multipilot");
static const auto TAILS_MULTIENGINE      = QLatin1String("multiengine");
static const auto TAILS_ENGINETYPE       = QLatin1String("engineType");
static const auto TAILS_WEIGHTCLASS      = QLatin1String("weightClass");

// pilots table

static const auto PILOTS_LASTNAME        = QLatin1String("lastname");
static const auto PILOTS_FIRSTNAME       = QLatin1String("firstname");
static const auto PILOTS_ALIAS           = QLatin1String("alias");
static const auto PILOTS_COMPANY         = QLatin1String("company");
static const auto PILOTS_EMPLOYEEID      = QLatin1String("employeeid");
static const auto PILOTS_PHONE           = QLatin1String("phone");
static const auto PILOTS_EMAIL           = QLatin1String("email");

// Currencies table
static const auto CURRENCIES_EXPIRYDATE  = QLatin1String("expiryDate");
static const auto CURRENCIES_DESCRIPTION = QLatin1String("description");

// all tables
static const auto ROWID                  = QLatin1String("ROWID");
static const auto NULL_TIME_hhmm         = QLatin1String("00:00");

static const auto DEFAULT_FLIGHT_POSITION   = DataPosition(TABLE_FLIGHTS, 0);
static const auto DEFAULT_PILOT_POSITION    = DataPosition(TABLE_PILOTS, 0);
static const auto DEFAULT_TAIL_POSITION     = DataPosition(TABLE_TAILS, 0);
static const auto DEFAULT_AIRCRAFT_POSITION = DataPosition(TABLE_AIRCRAFT, 0);

} // namespace opl::db

namespace Assets {

static const auto LOGO                          = QLatin1String(":/icons/opl-icons/logos/logo_text.png");
static const auto ICON_MAIN                     = QLatin1String(":/icons/opl-icons/app/icon_main.png");
static const auto ICON_APPICON_LINUX            = QLatin1String(":/icons/opl-icons/app/icon_linux.svg");
static const auto ICON_APPICON_IOS              = QLatin1String(":/icons/opl-icons/app/icon_ios.svg");
static const auto ICON_APPICON_WIN              = QLatin1String(":/icons/opl-icons/app/icon_windows.ico");

static const auto ICON_TOOLBAR_HOME             = QLatin1String(":/icons/opl-icons/toolbar/thick/light/icon_home.svg");
static const auto ICON_TOOLBAR_NEW_FLIGHT       = QLatin1String(":/icons/opl-icons/toolbar/thick/light/icon_new_flight.svg");
static const auto ICON_TOOLBAR_LOGBOOK          = QLatin1String(":/icons/opl-icons/toolbar/thick/light/icon_logbook.svg");
static const auto ICON_TOOLBAR_AIRCRAFT         = QLatin1String(":/icons/opl-icons/toolbar/thick/light/icon_airplane.svg");
static const auto ICON_TOOLBAR_PILOT            = QLatin1String(":/icons/opl-icons/toolbar/thick/light/icon_pilot.svg");
static const auto ICON_TOOLBAR_SETTINGS         = QLatin1String(":/icons/opl-icons/toolbar/thick/light/icon_settings.svg");
static const auto ICON_TOOLBAR_QUIT             = QLatin1String(":/icons/opl-icons/toolbar/thick/light/icon_exit.svg");

static const auto ICON_TOOLBAR_BACKUP           = QLatin1String(":/icons/opl-icons/toolbar/thick/light/icon_backup.svg");

static const auto ICON_TOOLBAR_HOME_DARK        = QLatin1String(":/icons/opl-icons/toolbar/thick/dark/icon_home_dm.svg");
static const auto ICON_TOOLBAR_NEW_FLIGHT_DARK  = QLatin1String(":/icons/opl-icons/toolbar/thick/dark/icon_new_flight_dm.svg");
static const auto ICON_TOOLBAR_LOGBOOK_DARK     = QLatin1String(":/icons/opl-icons/toolbar/thick/dark/icon_logbook_dm.svg");
static const auto ICON_TOOLBAR_AIRCRAFT_DARK    = QLatin1String(":/icons/opl-icons/toolbar/thick/dark/icon_airplane_dm.svg");
static const auto ICON_TOOLBAR_PILOT_DARK       = QLatin1String(":/icons/opl-icons/toolbar/thick/dark/icon_pilot_dm.svg");
static const auto ICON_TOOLBAR_SETTINGS_DARK    = QLatin1String(":/icons/opl-icons/toolbar/thick/dark/icon_settings_dm.svg");
static const auto ICON_TOOLBAR_QUIT_DARK        = QLatin1String(":/icons/opl-icons/toolbar/thick/dark/icon_exit_dm.svg");

static const auto ICON_TOOLBAR_BACKUP_DARK      = QLatin1String(":/icons/opl-icons/toolbar/thick/dark/icon_backup_dm.svg");

}

} // namespace opl

#endif // OPLCONSTANTS_H
