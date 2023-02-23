/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2022 Felix Turowsky
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
#include <QComboBox>

#define APPNAME QStringLiteral("openPilotLog")
#define ORGNAME QStringLiteral("opl")
#define ORGDOMAIN QStringLiteral("https://github.com/fiffty-50/openpilotlog")
#define OPL_VERSION 0
#define OPL_SUBVERSION 1

#if OPL_VERSION < 1
    #define OPL_VERSION_STRING QString(QString::number(OPL_VERSION) + "." + QString::number(OPL_SUBVERSION)) + "-alpha"
#else
    #define OPL_VERSION_STRING QString(QString::number(OPL_VERSION) + "." + QString::number(OPL_SUBVERSION))
#endif


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
namespace OPL {

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

#define INFO(msg) OPL::ANotificationHandler::info(msg, this)  // Use for messages of interest to the user (will be displayed in GUI)
#define WARN(msg) OPL::ANotificationHandler::warn(msg, this)  // Use for warnings (will be displayed in GUI)
#define CRIT(msg) OPL::ANotificationHandler::crit(msg, this)  // Use for critical warnings (will be displayed in GUI)

/*!
 * \brief The ANotificationHandler class handles displaying of user-directed messages. It displays
 * information to the user in a QMessageBox and forwards the displayed message to ALog so it is written
 * to the console and log files. The INFO, WARN and CRIT makros provide convenient access.
 */
class ANotificationHandler {
public:
    static inline void info(const QString msg, QWidget *parent = nullptr){

        qInfo() << msg;
        auto mb = QMessageBox(QMessageBox::Information, QStringLiteral("Info"), msg, QMessageBox::StandardButton::Ok, parent);
        mb.exec();
    };
    static inline void warn(const QString msg, QWidget *parent = nullptr){
        qWarning() << msg;
        auto mb = QMessageBox(QMessageBox::Warning, QStringLiteral("Warning"), msg, QMessageBox::StandardButton::Ok, parent);
        mb.exec();
    };
    static inline void crit(const QString msg, QWidget *parent = nullptr){
        qCritical() << msg;
        auto mb = QMessageBox(QMessageBox::Critical, QStringLiteral("Warning"), msg, QMessageBox::StandardButton::Ok, parent);
        mb.exec();
    };
}; // class ANotificationHandler

using RowData_T = QHash<QString, QVariant>;

/*!
 * \brief ADateFormats enumerates the accepted date formats for QDateEdits
 * \todo At the moment, only ISODate is accepet as a valid date format.
 */
enum class DateFormat {ISODate, DE, EN };

enum class FlightTimeFormat {Default, Decimal};

enum class DateTimeFormat {Default, Backup};

/*!
 * \brief PilotFunction
 * Pilot in Command, Pilot in Command under Supervision, Second in Command (Co-Pilot), Dual, Flight Instructor
 */
enum class PilotFunction {PIC = 0, PICUS = 1, SIC = 2, DUAL = 3, FI = 4};

/*!
 * \brief Enumerates the available translations
 */
enum class Translation {English, German, Spanish};

/*!
 * \brief Enumerates the available SQL views in the database
 */
enum class DbViewName {Default, DefaultWithSim, Easa, EasaWithSim, SimulatorOnly};

/*!
 * \brief Enumerates the Simulator Types: Flight and Navigation Procedures Trainer 1/2, Flight Simulation Training Device
 */
enum class SimulatorType {FNPTI = 0, FNPTII = 1, FSTD = 2};

/*!
 * \brief Enumerates the tables in the database
 */
enum class DbTable {Any, Flights, Simulators, Pilots, Tails, Aircraft, Airports, Currencies, Changelog};

/*!
 * \brief Enumerates the currency names
 */
enum class CurrencyName {Licence = 1, TypeRating = 2, LineCheck = 3, Medical = 4, Custom1 = 5, Custom2 = 6};

/*!
 * \brief The OplGlobals class encapsulates non-POD globals to avoid making them static. It is available
 * as a global static object via the OPL::GLOBAL makro and may be used as if it were a pointer, guaranteed to be initialized exactly once.
 * For more information, see (Q_GLOBAL_STATIC)[https://doc.qt.io/qt-5/qglobalstatic.html#details]
 */
class OplGlobals : public QObject {
public:
    OplGlobals() = default;

    void fillLanguageComboBox(QComboBox *combo_box) const;
    void fillViewNamesComboBox(QComboBox *combo_box) const;
    void loadPilotFunctios(QComboBox *combo_box) const;
    void loadSimulatorTypes(QComboBox *combo_box) const;
    void loadApproachTypes(QComboBox *combo_box) const;

    inline const QStringList &getApproachTypes() const {return APPROACH_TYPES;}
    inline const QString getLanguageFilePath(Translation language) const {return L10N_FilePaths.value(language);}
    inline const QString getViewIdentifier(DbViewName view_name) const {return DATABASE_VIEWS.value(view_name);}
    inline const QString getDbTableName(DbTable table_name) const {return DB_TABLES.value(table_name);}

private:
    Q_OBJECT
    const static inline QMap<Translation, QString> L10N_FilePaths {
        {Translation::English, QStringLiteral("l10n/openpilotlog_en")},
        {Translation::German,  QStringLiteral("l10n/openpilotlog_de")},
        {Translation::Spanish, QStringLiteral("l10n/openpilotlog_es")},
    };
    const static inline QMap<Translation, QString> L10N_DisplayNames {
        {Translation::English, QStringLiteral("English")},
        {Translation::German,  QStringLiteral("Deutsch")},
        {Translation::Spanish, QStringLiteral("Español")},
    };
    const static inline QMap<DbViewName, QString> DATABASE_VIEWS = {
        {DbViewName::Default,        QStringLiteral("viewDefault")},
        {DbViewName::DefaultWithSim, QStringLiteral("viewDefaultSim")},
        {DbViewName::Easa,           QStringLiteral("viewEasa")},
        {DbViewName::EasaWithSim,    QStringLiteral("viewEasaSim")},
        {DbViewName::SimulatorOnly,  QStringLiteral("viewSimulators")},
    };
    const QMap<DbViewName, QString> DATABASE_VIEW_DISPLAY_NAMES = {
        {DbViewName::Default,        tr("Default")},
        {DbViewName::DefaultWithSim, tr("Default with Simulator")},
        {DbViewName::Easa,           tr("EASA-FCL")},
        {DbViewName::EasaWithSim,    tr("EASA-FCL with Simulator")},
        {DbViewName::SimulatorOnly,  tr("Simulator Sessions Only")},
    };
    const static inline QMap<PilotFunction, QString> PILOT_FUNCTIONS = {
        {PilotFunction::PIC,   QStringLiteral("PIC")},
        {PilotFunction::PICUS, QStringLiteral("PICUS")},
        {PilotFunction::SIC,   QStringLiteral("SIC")},
        {PilotFunction::DUAL,  QStringLiteral("DUAL")},
        {PilotFunction::FI,    QStringLiteral("FI")},
    };
    const static inline QMap<SimulatorType, QString> SIMULATOR_TYPES = {
        {SimulatorType::FNPTI,  QStringLiteral("FNPT I")},
        {SimulatorType::FNPTII, QStringLiteral("FNPT II")},
        {SimulatorType::FSTD,   QStringLiteral("FSTD")},
    };
    const static inline QMap<DbTable, QString> DB_TABLES = {
        {DbTable::Flights,      QStringLiteral("flights")},
        {DbTable::Simulators,   QStringLiteral("simulators")},
        {DbTable::Pilots,       QStringLiteral("pilots")},
        {DbTable::Tails,        QStringLiteral("tails")},
        {DbTable::Aircraft,     QStringLiteral("aircraft")},
        {DbTable::Airports,     QStringLiteral("airports")},
        {DbTable::Currencies,   QStringLiteral("currencies")},
        {DbTable::Changelog,    QStringLiteral("changelog")},
    };

    const static inline QStringList APPROACH_TYPES = {
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
};

//Make available as a global static
Q_GLOBAL_STATIC(OplGlobals, GLOBALS)

/*!
 *  The OPL::Db namespace provides string literals to programatically access the database
 *
 *  Example usage, do:
 *  newData.insert(OPL::Db::FLIGHTS_DEP, ui->deptLocLineEdit->text());
 *  newData.value(OPL::Db::AIRCRAFT_MULTIPILOT);
 *
 *  instead of:
 *  newData.insert("dept", ui->deptLocLineEdit->text());
 *  newData.value("multipilot");
 *
 *  Declaring these literals here avoids memory allocation at runtime for construction of temporary
 *  qstrings like ("dept").
 */
namespace Db {


// Table names
const inline auto TABLE_FLIGHTS          = QStringLiteral("flights");
const inline auto TABLE_PILOTS           = QStringLiteral("pilots");
const inline auto TABLE_TAILS            = QStringLiteral("tails");
const inline auto TABLE_AIRCRAFT         = QStringLiteral("aircraft");
const inline auto TABLE_AIRPORTS         = QStringLiteral("airports");
const inline auto TABLE_CURRENCIES       = QStringLiteral("currencies");
const inline auto TABLE_SIMULATORS       = QStringLiteral("simulators");

// Flights table columns
const inline auto FLIGHTS_ROWID          = QStringLiteral("flight_id");
const inline auto FLIGHTS_DOFT           = QStringLiteral("doft");
const inline auto FLIGHTS_DEPT           = QStringLiteral("dept");
const inline auto FLIGHTS_DEST           = QStringLiteral("dest");
const inline auto FLIGHTS_TOFB           = QStringLiteral("tofb");
const inline auto FLIGHTS_TONB           = QStringLiteral("tonb");
const inline auto FLIGHTS_PIC            = QStringLiteral("pic");
const inline auto FLIGHTS_ACFT           = QStringLiteral("acft");
const inline auto FLIGHTS_TBLK           = QStringLiteral("tblk");
const inline auto FLIGHTS_TSPSE          = QStringLiteral("tSPSE");
const inline auto FLIGHTS_TSPME          = QStringLiteral("tSPME");
const inline auto FLIGHTS_TMP            = QStringLiteral("tMP");
const inline auto FLIGHTS_TNIGHT         = QStringLiteral("tNIGHT");
const inline auto FLIGHTS_TIFR           = QStringLiteral("tIFR");
const inline auto FLIGHTS_TPIC           = QStringLiteral("tPIC");
const inline auto FLIGHTS_TPICUS         = QStringLiteral("tPICUS");
const inline auto FLIGHTS_TSIC           = QStringLiteral("tSIC");
const inline auto FLIGHTS_TDUAL          = QStringLiteral("tDUAL");
const inline auto FLIGHTS_TFI            = QStringLiteral("tFI");
const inline auto FLIGHTS_TSIM           = QStringLiteral("tSIM");
const inline auto FLIGHTS_PILOTFLYING    = QStringLiteral("pilotFlying");
const inline auto FLIGHTS_TODAY          = QStringLiteral("toDay");
const inline auto FLIGHTS_TONIGHT        = QStringLiteral("toNight");
const inline auto FLIGHTS_LDGDAY         = QStringLiteral("ldgDay");
const inline auto FLIGHTS_LDGNIGHT       = QStringLiteral("ldgNight");
const inline auto FLIGHTS_AUTOLAND       = QStringLiteral("autoland");
const inline auto FLIGHTS_SECONDPILOT    = QStringLiteral("secondPilot");
const inline auto FLIGHTS_THIRDPILOT     = QStringLiteral("thirdPilot");
const inline auto FLIGHTS_APPROACHTYPE   = QStringLiteral("approachType");
const inline auto FLIGHTS_FLIGHTNUMBER   = QStringLiteral("flightNumber");
const inline auto FLIGHTS_REMARKS        = QStringLiteral("remarks");

// tails table

const inline auto TAILS_ROWID            = QStringLiteral("tail_id");
const inline auto TAILS_REGISTRATION     = QStringLiteral("registration");
const inline auto TAILS_COMPANY          = QStringLiteral("company");
const inline auto TAILS_MAKE             = QStringLiteral("make");
const inline auto TAILS_MODEL            = QStringLiteral("model");
const inline auto TAILS_VARIANT          = QStringLiteral("variant");
const inline auto TAILS_MULTIPILOT       = QStringLiteral("multipilot");
const inline auto TAILS_MULTIENGINE      = QStringLiteral("multiengine");
const inline auto TAILS_ENGINETYPE       = QStringLiteral("engineType");
const inline auto TAILS_WEIGHTCLASS      = QStringLiteral("weightClass");

// pilots table

const inline auto  PILOTS_ROWID           = QStringLiteral("pilot_id");
const inline auto  PILOTS_LASTNAME        = QStringLiteral("lastname");
const inline auto  PILOTS_FIRSTNAME       = QStringLiteral("firstname");
const inline auto  PILOTS_ALIAS           = QStringLiteral("alias");
const inline auto  PILOTS_COMPANY         = QStringLiteral("company");
const inline auto  PILOTS_EMPLOYEEID      = QStringLiteral("employeeid");
const inline auto  PILOTS_PHONE           = QStringLiteral("phone");
const inline auto  PILOTS_EMAIL           = QStringLiteral("email");

// Currencies table
const inline auto  CURRENCIES_EXPIRYDATE  = QStringLiteral("expiryDate");
const inline auto  CURRENCIES_CURRENCYNAME = QStringLiteral("currencyName");

// Simulators table
const inline auto  SIMULATORS_ROWID       = QStringLiteral("session_id");
const inline auto  SIMULATORS_DATE        = QStringLiteral("date");
const inline auto  SIMULATORS_TIME        = QStringLiteral("totalTime");
const inline auto  SIMULATORS_TYPE        = QStringLiteral("deviceType");
const inline auto  SIMULATORS_ACFT        = QStringLiteral("aircraftType");
const inline auto  SIMULATORS_REG         = QStringLiteral("registration");
const inline auto  SIMULATORS_REMARKS     = QStringLiteral("remarks");

// Airports table
const inline auto AIRPORTS_ICAO           = QStringLiteral("icao");
const inline auto AIRPORTS_IATA           = QStringLiteral("iata");
const inline auto AIRPORTS_NAME           = QStringLiteral("name");
const inline auto AIRPORTS_LAT            = QStringLiteral("lat");
const inline auto AIRPORTS_LON            = QStringLiteral("long");
const inline auto AIRPORTS_COUNTRY        = QStringLiteral("country");
const inline auto AIRPORTS_ALTITIDUE      = QStringLiteral("alt");
const inline auto AIRPORTS_UTC_OFFSET     = QStringLiteral("utcoffset");
const inline auto AIRPORTS_TZ_OLSON       = QStringLiteral("tzolson");

// all tables
const inline auto  ROWID                  = QStringLiteral("rowid");
const inline auto  NULL_TIME_hhmm         = QStringLiteral("00:00");

} // namespace OPL::db

namespace Assets {

const inline auto  DATABASE_SCHEMA               = QStringLiteral(":/database/database_schema.sql");
const inline auto  DATABASE_TEMPLATE_AIRCRAFT    = QStringLiteral(":/database/templates/aircraft.json");
const inline auto  DATABASE_TEMPLATE_AIRPORT     = QStringLiteral(":/database/templates/airports.json");
const inline auto  DATABASE_TEMPLATE_CHANGELOG   = QStringLiteral(":/database/templates/changelog.json");
const inline auto  DATABASE_TEMPLATE_CURRENCIES  = QStringLiteral(":/database/templates/currencies.json");

const inline auto  LOGO                          = QStringLiteral(":/icons/opl-icons/logos/logo_text.png");
const inline auto  ICON_MAIN                     = QStringLiteral(":/icons/opl-icons/app/icon_main.png");
const inline auto  ICON_APPICON_LINUX            = QStringLiteral(":/icons/opl-icons/app/icon_linux.svg");
const inline auto  ICON_APPICON_IOS              = QStringLiteral(":/icons/opl-icons/app/icon_ios.icns");
const inline auto  ICON_APPICON_WIN              = QStringLiteral(":/icons/opl-icons/app/icon_windows.ico");

const inline auto  ICON_TOOLBAR_HOME             = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_home.svg");
const inline auto  ICON_TOOLBAR_NEW_FLIGHT       = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_new_flight.svg");
const inline auto  ICON_TOOLBAR_LOGBOOK          = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_logbook.svg");
const inline auto  ICON_TOOLBAR_AIRCRAFT         = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_airplane.svg");
const inline auto  ICON_TOOLBAR_PILOT            = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_pilot.svg");
const inline auto  ICON_TOOLBAR_SETTINGS         = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_settings.svg");
const inline auto  ICON_TOOLBAR_QUIT             = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_exit.svg");

const inline auto  ICON_TOOLBAR_BACKUP           = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_backup.svg");

const inline auto  ICON_TOOLBAR_HOME_DARK        = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_home_dm.svg");
const inline auto  ICON_TOOLBAR_NEW_FLIGHT_DARK  = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_new_flight_dm.svg");
const inline auto  ICON_TOOLBAR_LOGBOOK_DARK     = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_logbook_dm.svg");
const inline auto  ICON_TOOLBAR_AIRCRAFT_DARK    = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_airplane_dm.svg");
const inline auto  ICON_TOOLBAR_PILOT_DARK       = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_pilot_dm.svg");
const inline auto  ICON_TOOLBAR_SETTINGS_DARK    = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_settings_dm.svg");
const inline auto  ICON_TOOLBAR_QUIT_DARK        = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_exit_dm.svg");

const inline auto  ICON_TOOLBAR_BACKUP_DARK      = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_backup_dm.svg");

}

namespace Styles {

const inline auto  RED_BORDER = QStringLiteral("border: 1px solid red");
} // namespace Styles

namespace Format {

const inline auto TIME_FORMAT = QStringLiteral("hh:mm");

} // namespace Format

} // namespace opl

#endif // OPLCONSTANTS_H
