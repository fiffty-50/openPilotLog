/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2023 Felix Turowsky
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

/**
 * @brief Defines the row ID for non-user entries in the database;
 */
const static int STUB_ROW_ID = -1;

/**
 * @brief Defines a four-letter code for a non-extistent (dummy) airport: "XXXX"
 */
const static char* STUB_AIRPORT_CODE = "XXXX";
/**
 * @brief Defines a registration for a non-existent (dummy) aircraft: "XX-XXX"
 */
const static char* STUB_AIRCRAFT_REG = "XX-XXX";

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

struct ToLdgCount_T {
    int toDay;
    int toNight;
    int ldgDay;
    int ldgNight;

    ToLdgCount_T(int toDay, int toNight, int ldgDay, int ldgNight)
        : toDay(toDay), toNight(toNight), ldgDay(ldgDay), ldgNight(ldgNight) {}
};

/*!
 * \brief The DateFormat struct encapsulates how date and time values are displayed.
 * \details Stores how the user wishes to display and enter Date and Time Entries.
 * These are stored numerically in the database and thus need to be converted to
 * human-readably form.
 */
struct DateTimeFormat {
    /*!
     * \brief Enumerates how dates can be formatted to a localised format.
     * \value Default - The Application standard, Equivalent to Qt::ISODate
     * \value SystemLocale - The current system locale date format
     */
    enum class DateFormat { Default, SystemLocale, Custom };

    /*!
     * \brief Enumerates how time values can be formatted
     * \value Default - The application default is 'hh:mm'
     * \value Decimal - Time as Decmial hours (01:30 == 1.5)
     * \value Custom - A user-provided custom format string
     */
    enum class TimeFormat { Default, Decimal, Custom };

    /*!
     * \brief Initialise a DateTimeFormat instance with default values
     */
    DateTimeFormat()
        : m_dateFormat(DateFormat::Default),
        m_dateFormatString(QStringLiteral("yyyy-MM-dd")),
        m_timeFormat(TimeFormat::Default),
        m_timeFormatString(QStringLiteral("hh:mm"))
    {}

    DateTimeFormat(DateFormat dateFormat_,
                   const QString &dateFormatString_,
                   TimeFormat timeFormat_,
                   const QString &timeFormatString_)
        :
        m_dateFormat(dateFormat_),
        m_dateFormatString(dateFormatString_),
        m_timeFormat(timeFormat_),
        m_timeFormatString(timeFormatString_)
    {}


public:
    DateFormat dateFormat() const { return m_dateFormat; }
    TimeFormat timeFormat() const { return m_timeFormat; }
    const QString &dateFormatString() const { return m_dateFormatString; }
    const QString &timeFormatString() const { return m_timeFormatString; }

private:
    DateFormat m_dateFormat;
    TimeFormat m_timeFormat;
    QString m_dateFormatString;
    QString m_timeFormatString;
};

/*!
 * \brief ADateFormats enumerates the accepted date formats for QDateEdits
 * \todo At the moment, only ISODate is accepet as a valid date format.
 */
enum class DateFormat {ISODate, DE, EN };

enum class FlightTimeFormat {Default, Decimal};

enum class DateTimeFormat_deprecated {Default, Backup};

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
enum class LogbookView {Default, DefaultWithSim, Easa, EasaWithSim, SimulatorOnly};

/*!
 * \brief Enumerates the Simulator Types: Flight and Navigation Procedures Trainer 1/2, Flight Simulation Training Device
 */
enum class SimulatorType {FNPTI = 0, FNPTII = 1, FSTD = 2};

/*!
 * \brief Enumerates the tables in the database
 */
enum class DbTable {Any, Flights, Simulators, Pilots, Tails, Aircraft, Airports, Currencies, Changelog, PreviousExperience};

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
    inline const QString getViewIdentifier(LogbookView view_name) const {return DATABASE_VIEWS.value(view_name);}
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
    const static inline QMap<LogbookView, QString> DATABASE_VIEWS = {
        {LogbookView::Default,        QStringLiteral("viewDefault")},
        {LogbookView::DefaultWithSim, QStringLiteral("viewDefaultSim")},
        {LogbookView::Easa,           QStringLiteral("viewEasa")},
        {LogbookView::EasaWithSim,    QStringLiteral("viewEasaSim")},
        {LogbookView::SimulatorOnly,  QStringLiteral("viewSimulators")},
    };
    const QMap<LogbookView, QString> DATABASE_VIEW_DISPLAY_NAMES = {
        {LogbookView::Default,        tr("Default")},
        {LogbookView::DefaultWithSim, tr("Default with Simulator")},
        {LogbookView::Easa,           tr("EASA-FCL")},
        {LogbookView::EasaWithSim,    tr("EASA-FCL with Simulator")},
        {LogbookView::SimulatorOnly,  tr("Simulator Sessions Only")},
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
        {DbTable::PreviousExperience,    QStringLiteral("previousExperience")},
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


namespace Assets {

const inline auto  DATABASE_SCHEMA               = QStringLiteral(":/database/database_schema.sql");
const inline auto  DATABASE_TEMPLATE_AIRCRAFT    = QStringLiteral(":/database/templates/aircraft.json");
const inline auto  DATABASE_TEMPLATE_AIRPORT     = QStringLiteral(":/database/templates/airports.json");
const inline auto  DATABASE_TEMPLATE_CHANGELOG   = QStringLiteral(":/database/templates/changelog.json");

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

namespace CssStyles {

const inline auto  RED_BORDER = QStringLiteral("border: 1px solid red");
} // namespace Styles

namespace Format {

const inline auto TIME_FORMAT = QStringLiteral("hh:mm");

} // namespace Format

namespace RegEx {

const inline auto RX_PHONE_NUMBER  = QRegularExpression(QStringLiteral("^[+]{0,1}[0-9\\-\\s]+"));
const inline auto RX_EMAIL_ADDRESS = QRegularExpression(QStringLiteral("\\A[a-z0-9!#$%&'*+/=?^_‘{|}~-]+(?:\\.[a-z0-9!#$%&'*+/=?^_‘{|}~-]+)*@"));
//const inline auto RX_TIME_ENTRY    = QRegularExpression(QStringLiteral("([01]?[0-9]|2[0-3]):?[0-5][0-9]?"));
const inline auto RX_TIME_ENTRY    = QRegularExpression(QStringLiteral("^(?:(?:([01]?\\d|2[0-3])(?::?)([0-5]\\d))|(?:([01]?\\d|2[0-3])([0-5]\\d))|(?:([1-9]|[1-9]\\d)\\:([0-5]\\d)?)|(?:([01]?\\d|2[0-3])\\.([0-5]?\\d)))$"));
const inline auto RX_AIRPORT_CODE  = QRegularExpression(QStringLiteral("[a-zA-Z0-9]{1,4}"));

} // namespace RegEx

} // namespace opl

#endif // OPLCONSTANTS_H
