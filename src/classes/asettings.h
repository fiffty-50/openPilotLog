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
#ifndef ASETTINGS_H
#define ASETTINGS_H
#include <QtCore>
#include <QSettings>

/*!
 * \brief Thin wrapper for the QSettings class,
 * simplifying reading and writing of settings.
 */
class ASettings {
public:
    enum class Main {
        SetupComplete,
        Style,
        Font,
        FontSize,
        UseSystemFont,
        LogbookView,
        DateFormat,
    };

    enum class UserData {
        DisplaySelfAs,
        TailSortColumn,
        PilotSortColumn,
        AcftAllowIncomplete,
        FtlWarningThreshold,
        CurrWarningEnabled,
        CurrWarningThreshold,
        ShowToLgdCurrency,
        ShowLicCurrency,
        ShowTrCurrency,
        ShowLckCurrency,
        ShowMedCurrency,
        ShowCustom1Currency,
        ShowCustom2Currency,
        Custom1CurrencyName,
        Custom2CurrencyName,
    };

    enum class FlightLogging {
        Function,
        Approach,
        NightLoggingEnabled,
        LogIFR,
        FlightNumberPrefix,
        NumberTakeoffs,
        NumberLandings,
        PopupCalendar,
        PilotFlying,
        NightAngle,
        Rules,
        FlightTimeFormat,
        FunctionComboBox,
        CalendarCheckBox,
    };

    /*!
     * \brief Should be called after QCoreApplication::set...Name have been called.
     */
    static void setup();
    static void resetToDefaults();

    static QVariant read(const Main key);
    static void write(const Main key, const QVariant &val);

    static QVariant read(const FlightLogging key);
    static void write(const UserData key, const QVariant &val);

    static QVariant read(const UserData key);
    static void write(const FlightLogging key, const QVariant &val);

    /*!
     * \brief Return string representation of group of key: "ini_header/key"
     */
    static QString groupOfKey(const Main key);
    static QString groupOfKey(const FlightLogging key);
    static QString groupOfKey(const UserData key);

    /*!
     * \brief Return string representation of key
     */
    static QString stringOfKey(const Main key);
    static QString stringOfKey(const FlightLogging key);
    static QString stringOfKey(const UserData key);

    static QSettings settings();

private:
    static QMap<Main, QString> mainMap;
    static QMap<UserData, QString> userDataMap;
    static QMap<FlightLogging, QString> flightLoggingMap;
};

#endif // ASETTINGS_H
