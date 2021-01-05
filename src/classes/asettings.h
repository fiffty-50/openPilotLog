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
#include <QtGlobal>
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

#if QT_VERSION > QT_VERSION_CHECK(5, 9, 0)
    static QSettings settings() { return QSettings(); }
#endif

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


private:
    static QMap<Main, QString> mainMap;
    static QMap<LogBook, QString> logBookMap;
    static QMap<UserData, QString> userDataMap;
    static QMap<FlightLogging, QString> flightLoggingMap;
    static QMap<Setup, QString> setupMap;
    static QMap<NewFlight, QString> newFlightMap;

#define FOR_CURRENT_VERSION 0
#if (FOR_CURRENT_VERSION)
    /* By default unless specified each enum is + 1.
       If we are interested in doing multiple returns at once then
       we should make them so that the can be OR-ed.

        That means that in binary this means that
        every new enum has only 1 bit on
        that bit is always moving to the left
    */
    enum Setup2 {
        SetupComplete       = 0x00'00'00'01,  // If this makes no sense look up hexadecimals
    };                                        // the ' is legal in numbers it is ignored
                                              // used only for reader convinience
    enum Main2 {
        Theme               = 0x00'00'00'02,
        ThemeID             = 0x00'00'00'04,
    };

    enum LogBook2 {
        View                = 0x00'00'00'08,
    };

    enum UserData2 {
        LastName            = 0x00'00'00'10,
        FirstName           = 0x00'00'00'20,
        Company             = 0x00'00'00'40,
        EmployeeID          = 0x00'00'00'80,
        Phone               = 0x00'00'01'00,
        Email               = 0x00'00'02'00,
        DisplaySelfAs       = 0x00'00'04'00,
        Alias               = 0x00'00'08'00,
        AcSortColumn        = 0x00'00'10'00,
        PilSortColumn       = 0x00'00'20'00,
        AcAllowIncomplete   = 0x00'00'40'00,
    };

    enum FlightLogging2 {
        Function            = 0x00'00'80'00,
        Approach            = 0x00'01'00'00,
        NightLogging        = 0x00'02'00'00,
        LogIFR              = 0x00'04'00'00,
        FlightNumberPrefix  = 0x00'08'00'00,
        NumberTakeoffs      = 0x00'10'00'00,
        NumberLandings      = 0x00'20'00'00,
        PopupCalendar       = 0x00'40'00'00,
        PilotFlying         = 0x00'80'00'00,
        NightAngle          = 0x01'00'00'00,
        Rules               = 0x02'00'00'00,
    };

    enum NewFlight2 {
        FunctionComboBox    = 0x04'00'00'00,
        CalendarCheckBox    = 0x08'00'00'00,
    };

    using Key = int;
    using Keys = int;
    /*
     * Used like QMessageBox buttons;
     *
     * auto str = stringOfKey2(ASettings::Setup);
     * auto strlist = stringOfKeys2(ASettings::Setup | ASettings::Function)
     */
    QString stringOfKey2(Key k);
    QStringList stringOfKeys2(Keys ks);

#endif
#undef FOR_CURRENT_VERSION

};

#endif // ASETTINGS_H
