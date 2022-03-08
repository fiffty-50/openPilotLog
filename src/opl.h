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
#include <QComboBox>
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
namespace OPL {

#define OPL_VERSION 0
#define OPL_SUBVERSION 1
#define OPL_VERSION_STRING QString(QString::number(OPL_VERSION) + "." + QString::number(OPL_SUBVERSION))

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
        auto mb = QMessageBox(QMessageBox::Information, QLatin1String("Info"), msg, QMessageBox::StandardButton::Ok, parent);
        mb.exec();
    };
    static inline void warn(const QString msg, QWidget *parent = nullptr){
        qWarning() << msg;
        auto mb = QMessageBox(QMessageBox::Warning, QLatin1String("Warning"), msg, QMessageBox::StandardButton::Ok, parent);
        mb.exec();
    };
    static inline void crit(const QString msg, QWidget *parent = nullptr){
        qCritical() << msg;
        auto mb = QMessageBox(QMessageBox::Critical, QLatin1String("Warning"), msg, QMessageBox::StandardButton::Ok, parent);
        mb.exec();
    };
}; // class ANotificationHandler

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
enum PilotFunction {PIC = 0, PICUS = 1, SIC = 2, DUAL = 3, FI = 4};

/*!
 * \brief Enumerates the available translations
 */
enum Translation {English, German, Spanish};

/*!
 * \brief Enumerates the available SQL views in the database
 */
enum DbViewName {Default, DefaultWithSim, Easa, EasaWithSim};

/*!
 * \brief Enumerates the Simulator Types: Flight and Navigation Procedures Trainer 1/2, Flight Simulation Training Device
 */
enum SimulatorType {FNPTI = 0, FNPTII = 1, FSTD = 2};

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

private:
    Q_OBJECT

    const QMap<Translation, QString> L10N_FilePaths {
        {Translation::English, QStringLiteral("l10n/openpilotlog_en")},
        {Translation::German,  QStringLiteral("l10n/openpilotlog_de")},
        {Translation::Spanish, QStringLiteral("l10n/openpilotlog_es")},
    };
    const QMap<Translation, QString> L10N_DisplayNames {
        {Translation::English, tr("English")},
        {Translation::German,  tr("Deutsch")},
        {Translation::Spanish, tr("Español")},
    };
    const QMap<DbViewName, QString> DATABASE_VIEWS = {
        {Default,        QStringLiteral("viewDefault")},
        {DefaultWithSim, QStringLiteral("viewDefaultSim")},
        {Easa,           QStringLiteral("viewEasa")},
        {EasaWithSim,    QStringLiteral("viewEasaSim")},
    };
    const QMap<DbViewName, QString> DATABASE_VIEW_DISPLAY_NAMES = {
        {Default,        tr("Default")},
        {DefaultWithSim, tr("Default with Simulator")},
        {Easa,           tr("EASA-FCL")},
        {EasaWithSim,    tr("EASA-FCL with Simulator")},
    };
    const QMap<PilotFunction, QLatin1String> PILOT_FUNCTIONS = {
        {PilotFunction::PIC,   QLatin1String("PIC")},
        {PilotFunction::PICUS, QLatin1String("PICUS")},
        {PilotFunction::SIC,   QLatin1String("SIC")},
        {PilotFunction::DUAL,  QLatin1String("DUAL")},
        {PilotFunction::FI,    QLatin1String("FI")},
    };
    const QMap<SimulatorType, QString> SIMULATOR_TYPES = {
        {FNPTI,  QStringLiteral("FNPT I")},
        {FNPTII, QStringLiteral("FNPT II")},
        {FSTD,   QStringLiteral("FSTD")},
    };
    const QStringList APPROACH_TYPES = {
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
 *  The OPL::db namespace provides string literals to programatically access the database
 *
 *  Example usage, do:
 *  newData.insert(OPL::db::FLIGHTS_DEP, ui->deptLocLineEdit->text());
 *  newData.value(OPL::db::AIRCRAFT_MULTIPILOT);
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
static const auto TABLE_CURRENCIES       = QStringLiteral("currencies");
static const auto TABLE_SIMULATORS       = QStringLiteral("simulators");

// Flights table columns
static const auto FLIGHTS_ROWID          = QStringLiteral("flight_id");
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

static const auto TAILS_ROWID            = QStringLiteral("tail_id");
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

static const auto PILOTS_ROWID           = QStringLiteral("pilot_id");
static const auto PILOTS_LASTNAME        = QStringLiteral("lastname");
static const auto PILOTS_FIRSTNAME       = QStringLiteral("firstname");
static const auto PILOTS_ALIAS           = QStringLiteral("alias");
static const auto PILOTS_COMPANY         = QStringLiteral("company");
static const auto PILOTS_EMPLOYEEID      = QStringLiteral("employeeid");
static const auto PILOTS_PHONE           = QStringLiteral("phone");
static const auto PILOTS_EMAIL           = QStringLiteral("email");

// Currencies table
static const auto CURRENCIES_EXPIRYDATE  = QStringLiteral("expiryDate");
static const auto CURRENCIES_DESCRIPTION = QStringLiteral("description");

// Simulators table
static const auto SIMULATORS_ROWID       = QStringLiteral("session_id");
static const auto SIMULATORS_DATE        = QStringLiteral("date");
static const auto SIMULATORS_TIME        = QStringLiteral("totalTime");
static const auto SIMULATORS_TYPE        = QStringLiteral("deviceType");
static const auto SIMULATORS_ACFT        = QStringLiteral("aircraftType");
static const auto SIMULATORS_REG         = QStringLiteral("registration");
static const auto SIMULATORS_REMARKS     = QStringLiteral("remarks");

// all tables
static const auto ROWID                  = QStringLiteral("rowid");
static const auto NULL_TIME_hhmm         = QStringLiteral("00:00");

} // namespace OPL::db

namespace Assets {

static const auto LOGO                          = QStringLiteral(":/icons/opl-icons/logos/logo_text.png");
static const auto ICON_MAIN                     = QStringLiteral(":/icons/opl-icons/app/icon_main.png");
static const auto ICON_APPICON_LINUX            = QStringLiteral(":/icons/opl-icons/app/icon_linux.svg");
static const auto ICON_APPICON_IOS              = QStringLiteral(":/icons/opl-icons/app/icon_ios.svg");
static const auto ICON_APPICON_WIN              = QStringLiteral(":/icons/opl-icons/app/icon_windows.ico");

static const auto ICON_TOOLBAR_HOME             = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_home.svg");
static const auto ICON_TOOLBAR_NEW_FLIGHT       = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_new_flight.svg");
static const auto ICON_TOOLBAR_LOGBOOK          = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_logbook.svg");
static const auto ICON_TOOLBAR_AIRCRAFT         = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_airplane.svg");
static const auto ICON_TOOLBAR_PILOT            = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_pilot.svg");
static const auto ICON_TOOLBAR_SETTINGS         = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_settings.svg");
static const auto ICON_TOOLBAR_QUIT             = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_exit.svg");

static const auto ICON_TOOLBAR_BACKUP           = QStringLiteral(":/icons/opl-icons/toolbar/thick/light/icon_backup.svg");

static const auto ICON_TOOLBAR_HOME_DARK        = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_home_dm.svg");
static const auto ICON_TOOLBAR_NEW_FLIGHT_DARK  = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_new_flight_dm.svg");
static const auto ICON_TOOLBAR_LOGBOOK_DARK     = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_logbook_dm.svg");
static const auto ICON_TOOLBAR_AIRCRAFT_DARK    = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_airplane_dm.svg");
static const auto ICON_TOOLBAR_PILOT_DARK       = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_pilot_dm.svg");
static const auto ICON_TOOLBAR_SETTINGS_DARK    = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_settings_dm.svg");
static const auto ICON_TOOLBAR_QUIT_DARK        = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_exit_dm.svg");

static const auto ICON_TOOLBAR_BACKUP_DARK      = QStringLiteral(":/icons/opl-icons/toolbar/thick/dark/icon_backup_dm.svg");

}

namespace Styles {

static const auto RED_BORDER = QStringLiteral("border: 1px solid red");
} // namespace Styles

} // namespace opl

#endif // OPLCONSTANTS_H
