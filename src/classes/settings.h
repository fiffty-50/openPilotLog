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
#ifndef SETTINGS_H
#define SETTINGS_H
#include "src/opl.h"
#include <QtCore>
#include <QSettings>

/*!
 * \brief A wrapper for the QSettings class, simplifying reading and writing of settings.
 */
class Settings {
public:
    /*!
     * \brief Initialise the default setting object. Call after QCoreApplication has been set up in main
     */
    static void init();

    /*!
     * \brief Reset the application settings to its default values
     */
    static void resetToDefaults();

    /*!
     * \brief Writes any unsaved changes to permanent storage
     * \note This function is called automatically from QSettings's destructor and by the event loop at regular intervals,
     * so you normally don't need to call it yourself.
     */
    static void sync() { settingsInstance->sync(); }

    /*!
     * \brief Initial set-up of the application and database has been completed
     */
    static bool getSetupCompleted() { return settingsInstance->value(MAIN_SETUP_COMPLETE, false).toBool(); }

    /*!
     * \brief Initial set-up of the application and database has been completed
     */
    static void setSetupCompleted(bool completed) { settingsInstance->setValue(MAIN_SETUP_COMPLETE, completed); }

    /*!
     * \brief returns the name of the preferred application style (default: fusion)
     */
    static const QString getApplicationStyle() { return settingsInstance->value(MAIN_STYLE, "Fusion").toString(); }

    /*!
     * \brief set the name of the preferred application style
     */
    static const void setApplicationStyle(const QString &style) { settingsInstance->setValue(MAIN_STYLE, style); }

    /*!
     * \brief returns the name of the preferred application font (default: system font)
     * \return
     */
    static const QString getApplicationFontName() { return settingsInstance->value(MAIN_FONT_NAME, QString()).toString(); }

    /*!
     * \brief set the name of the preferred application font
     * \return
     */
    static const void setApplicationFontName(const QString &fontName) { settingsInstance->setValue(MAIN_FONT_NAME, fontName); }

    /*!
     * \brief returns the preferred font size (default: 10)
     * \return
     */
    static int getApplicationFontSize() { return settingsInstance->value(MAIN_FONT_SIZE, 10).toInt(); }

    /*!
     * \brief sets the preferred font size
     */
    static void setApplicationFontSize(int size) { settingsInstance->setValue(MAIN_FONT_SIZE, size); }

    /*!
     * \brief returns if the default system font should be used (default: true)
     */
    static bool getUseSystemFont() { return settingsInstance->value(MAIN_USE_SYSTEM_FONT, true).toBool(); }

    /*!
     * \brief sets if the default system font should be used
     */
    static void setUseSystemFont(bool value) { settingsInstance->setValue(MAIN_USE_SYSTEM_FONT, value); }

    /*!
     * \brief returns the view to be used in the logbook widget
     */
    static OPL::LogbookView getLogbookView() { return OPL::LogbookView(settingsInstance->value(MAIN_LOGBOOK_VIEW).toInt()); }

    /*!
     * \brief sets the view to be used in the logbook widget
     */
    static void setLogbookView(OPL::LogbookView view) { (settingsInstance->setValue(MAIN_LOGBOOK_VIEW, static_cast<int>(view))); }

    /*!
     * \brief returns the Display Format used in the application
     */
    static OPL::DateTimeFormat getDisplayFormat();

    /*!
     * \brief sets the Display Format used in the application
     */
    static void setDisplayFormat(const OPL::DateTimeFormat &format);

    /*!
     * \brief returns the default pilot function for new flights
     */
    static OPL::PilotFunction getPilotFunction() { return OPL::PilotFunction(settingsInstance->value(LOG_FUNCTION).toInt()); }

    /*!
     * \brief sets the default pilot function for new flights
     */
    static void setPilotFunction(OPL::PilotFunction function) { settingsInstance->setValue(LOG_FUNCTION, static_cast<int>(function)); }

    /*!
     * \brief returns the default approach type for new flights
     */
    static const QString getApproachType() { return settingsInstance->value(LOG_APPROACH).toString(); }
    /*!
     * \brief sets the default approach type for new flights
     */
    static void setApproachType(const QString &value) { settingsInstance->setValue(LOG_APPROACH, value); }

    /*!
     * \brief returns if automatic night time calculation is enabled for new flights
     */
    static bool getNightLoggingEnabled() { return settingsInstance->value(LOG_NIGHT).toBool(); }

    /*!
     * \brief sets if automatic night time calculation is enabled for new flights
     */
    static void setNightLoggingEnabled(bool value) { settingsInstance->setValue(LOG_NIGHT, value); }

    /*!
     * \brief returns the angle of elevation for night time calculation (default: -6 degrees)
     */
    static int getNightAngle() { return settingsInstance->value(LOG_NIGHT_ANGLE, -6).toInt(); }

    /*!
     * \brief sets the angle of elevation for night time calculation
     */
    static void setNightAngle(int value) { settingsInstance->setValue(LOG_NIGHT_ANGLE, value); }

    /*!
     * \brief returns if flight time should be logged as IFR for new flights
     */
    static bool getLogIfr() { return settingsInstance->value(LOG_IFR).toBool(); }

    /*!
     * \brief sets if flight time should be logged as IFR for new flights
     */
    static void setLogIfr(bool value) { settingsInstance->setValue(LOG_IFR, value); }

    /*!
     * \brief returns if new flights should be logged as Pilot Flying
     */
    static bool getLogAsPilotFlying() { return settingsInstance->value(LOG_AS_PF).toBool(); }

    /*!
     * \brief sets if new flights should be logged as Pilot Flying
     */
    static void setLogAsPilotFlying(bool value) { settingsInstance->setValue(LOG_AS_PF, value); }

    /*!
     * \brief returns the default Flight Number Prefix for new flights
     */
    static const QString getFlightNumberPrefix() { return settingsInstance->value(LOG_PREFIX).toString(); }

    /*!
     * \brief sets the default Flight Number Prefix for new flights
     */
    static void setFlightNumberPrefix(const QString &value) { settingsInstance->setValue(LOG_PREFIX, value); }

    /*!
     * \brief sets how the logbook owner is shown in the view
     * \details
     * <ul>
     * <li> 0 - self
     *
     */
    static int getShowSelfAs() { return settingsInstance->value(SHOW_SELF_AS).toInt(); }

    static void setShowSelfAs(int value) { settingsInstance->setValue(SHOW_SELF_AS, value); }

    static int getTailSortColumn() { return settingsInstance->value(TAIL_SORT_COLUMN).toInt(); }
    static void setTailSortColumn(int value) { settingsInstance->setValue(TAIL_SORT_COLUMN, value); }

    static int getPilotSortColumn() { return settingsInstance->value(PILOT_SORT_COLUMN).toInt(); }
    static void setPilotSortColumn(int value) { settingsInstance->setValue(PILOT_SORT_COLUMN, value); }

    static double getFtlWarningThreshold() { return settingsInstance->value(FTL_WARNING_THR, 0.8).toDouble(); }
    static void setFtlWarningThreshold(double value) { settingsInstance->setValue(FTL_WARNING_THR, value); }

    static int getCurrencyWarningThreshold() { return settingsInstance->value(CURR_WARNING_THR, 90).toInt(); }
    static void setCurrencyWarningThreshold(int days) { settingsInstance->setValue(CURR_WARNING_THR, days); }



private:

    // keep an instance to avoid having to create a new QSettings object every time
    static inline QSettings *settingsInstance;

    // Setting keys
    const static inline QString CURRENCY_STUB   	= QStringLiteral("userdata/%1Currency");
    const static inline QString SHOW_SELF_AS    	= QStringLiteral("userdata/displaySelfAs");
    const static inline QString TAIL_SORT_COLUMN    = QStringLiteral("userdata/tailSortColumn");
    const static inline QString PILOT_SORT_COLUMN	= QStringLiteral("userdata/pilotSortColumn");
    const static inline QString FTL_WARNING_THR		= QStringLiteral("ftlWarningThreshold");
    const static inline QString CURR_WARNING_THR	= QStringLiteral("currWarningThreshold");

    const static inline QString LOG_FUNCTION	= QStringLiteral("flightlogging/function");
    const static inline QString LOG_APPROACH	= QStringLiteral("flightlogging/approach");
    const static inline QString LOG_NIGHT	 	= QStringLiteral("flightlogging/nightLoggingEnabled");
    const static inline QString LOG_NIGHT_ANGLE	= QStringLiteral("flightlogging/nightangle");
    const static inline QString LOG_IFR	 	 	= QStringLiteral("flightlogging/logIfr");
    const static inline QString LOG_AS_PF	 	= QStringLiteral("flightlogging/pilotFlying");
    const static inline QString LOG_PREFIX	 	= QStringLiteral("flightlogging/flightnumberPrefix");

    const static inline QString MAIN_SETUP_COMPLETE  	= QStringLiteral("main/setupComplete");
    const static inline QString MAIN_STYLE			 	= QStringLiteral("main/style");
    const static inline QString MAIN_FONT_NAME		 	= QStringLiteral("main/font");
    const static inline QString MAIN_FONT_SIZE 		 	= QStringLiteral("main/fontSize");
    const static inline QString MAIN_USE_SYSTEM_FONT 	= QStringLiteral("main/useSystemFont");
    const static inline QString MAIN_LOGBOOK_VIEW 	 	= QStringLiteral("main/logbookView");

    const static inline QString FORMAT_DATE_FORMAT 	= QStringLiteral("format/dateFormat");
    const static inline QString FORMAT_DATE_STRING 	= QStringLiteral("format/dateFormatString");
    const static inline QString FORMAT_TIME_FORMAT	= QStringLiteral("format/timeFormat");
    const static inline QString FORMAT_TIME_STRING 	= QStringLiteral("format/timeFormatString");


};

#endif // SETTINGS_H
