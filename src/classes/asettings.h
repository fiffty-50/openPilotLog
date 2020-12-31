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
        Theme,
        ThemeID,
    };

    enum class LogBook {
        View,
    };

    enum class UserData {
        LastName,
        FirstName,
        Company,
        EmployeeID,
        Phone,
        Email,
        DisplaySelfAs,
        Alias,
        AcSortColumn, // not sure what ac is
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
    };

    enum class NewFlight {
        FunctionComboBox,
        CalendarCheckBox,
    };

    /*!
     * \brief Should be called after QCoreApplication::set...Name have been called.
     */
    static void setup();

    static inline QVariant read(const Main key);
    static inline void write(const Main key, const QVariant &val);

    static inline QVariant read(const FlightLogging key);
    static inline void write(const UserData key, const QVariant &val);

    static inline QVariant read(const UserData key);
    static inline void write(const FlightLogging key, const QVariant &val);

    static inline QVariant read(const Setup key);
    static inline void write(const Setup key, const QVariant &val);

    static inline QVariant read(const NewFlight key);
    static inline void write(const NewFlight key, const QVariant &val);

    static inline QString stringOfKey(const Main key);
    static inline QString stringOfKey(const UserData key);
    static inline QString stringOfKey(const FlightLogging key);
    static inline QString stringOfKey(const Setup key);

    static QSettings settings();

private:
    static QMap<Main, QString> mainMap;
    static QMap<LogBook, QString> logBookMap;
    static QMap<UserData, QString> userDataMap;
    static QMap<FlightLogging, QString> flightLoggingMap;
    static QMap<Setup, QString> setupMap;
    static QMap<NewFlight, QString> newFlightMap;

// [G]: no need for these anymore
//	static QVariant read(const QString &key);
//	static void write(const QString &key, const QVariant &val);

};

#endif // ASETTINGS_H
