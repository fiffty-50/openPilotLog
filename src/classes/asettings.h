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
#ifndef ASETTINGS_H
#define ASETTINGS_H
#include <QtCore>
#include <QSettings>

/*!
 * \brief Thin (namespace) wrapper for the QSettings class,
 * simplifying reading and writing of settings.
 */
class ASettings {
public:
    enum class Setup {
        SetupComplete,
    };

    enum class Main {
        Style,
        StyleSheet,
    };

    enum class LogBook {
        View,
    };

    enum class UserData {
        DisplaySelfAs,
        AcftSortColumn,
        PilSortColumn,
        AcAllowIncomplete,
    };

    enum class FlightLogging {
        Function,
        Approach,
        NightLogging,
        LogIFR,
        FlightNumberPrefix,
        NumberTakeoffs,
        NumberLandings,
        PopupCalendar,
        PilotFlying,
        NightAngle,
        Rules,
        FlightTimeFormat
    };

    enum class NewFlight {
        FunctionComboBox,
        CalendarCheckBox,
    };

    /*!
     * \brief Should be called after QCoreApplication::set...Name have been called.
     */
    static void setup();

    static QVariant read(const Setup key);
    static void write(const Setup key, const QVariant &val);

    static QVariant read(const LogBook key);
    static void write(const LogBook key, const QVariant &val);

    static QVariant read(const Main key);
    static void write(const Main key, const QVariant &val);

    static QVariant read(const FlightLogging key);
    static void write(const UserData key, const QVariant &val);

    static QVariant read(const UserData key);
    static void write(const FlightLogging key, const QVariant &val);

    static QVariant read(const NewFlight key);
    static void write(const NewFlight key, const QVariant &val);

    // [G]: enum class may be making it abit too strict perhaps?
    // a workaround is to use plain enums and have one function takes an int
    // All enums should be unique of course thats easy. See At the end of the file
    // for details
    /*!
     * \brief Return string representation of group of key: "ini_header/key"
     */
    static QString groupOfKey(const Main key);
    static QString groupOfKey(const LogBook key);
    static QString groupOfKey(const NewFlight key);
    static QString groupOfKey(const FlightLogging key);
    static QString groupOfKey(const Setup key);
    static QString groupOfKey(const UserData key);

    /*!
     * \brief Return string representation of key
     */
    static QString stringOfKey(const Main key);
    static QString stringOfKey(const LogBook key);
    static QString stringOfKey(const NewFlight key);
    static QString stringOfKey(const FlightLogging key);
    static QString stringOfKey(const Setup key);
    static QString stringOfKey(const UserData key);

    static QSettings settings();

private:
    static QMap<Main, QString> mainMap;
    static QMap<LogBook, QString> logBookMap;
    static QMap<UserData, QString> userDataMap;
    static QMap<FlightLogging, QString> flightLoggingMap;
    static QMap<Setup, QString> setupMap;
    static QMap<NewFlight, QString> newFlightMap;

};

#endif // ASETTINGS_H
