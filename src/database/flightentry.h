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
#ifndef FLIGHTENTRY_H
#define FLIGHTENTRY_H
#include "src/database/databasecache.h"
#include "src/database/row.h"
#include "src/gui/verification/validationstate.h"


namespace OPL {
/*!
 * \brief A Row representing a Flight entry
 * \details A FlightEntry represents a row in the flights table of the database and extends OPL::Row
 * It contains mandatory basic information like Departure, Destination and block times
 * but can also contain a number of additional items. The mandatory part is tracked
 * with the ValidationState Object contained in the entry.
 *
 * The FlightEntry class contains convenience functions that encapsulate logic for adding the different
 * items so that manual access to the OPL::RowData_T should not normally be required. Basic input validation
 * is done in the UI but the FlightEntry class runs some additional checks to confirm that data is not only
 * semantically correct.
 */
class FlightEntry : public Row
{
public:
    FlightEntry();
    FlightEntry(const RowData_T &row_data);
    FlightEntry(int row_id, const RowData_T &row_data);

    enum class PilotFunction {
        PIC = 0,
        PICUS = 1,
        SIC = 2,
        DUAL = 3,
        FI = 4
    };

    /*!
     * \brief a Flight Entry is valid if all its mandatory items are valid
     * \details In order to submit a flight entyr to the database, the foreign
     * key constraints of the database must be met. These constraints ensure the
     * minimum required data for a given flight entry is available. Check the
     * ValidationState for details.
     */
    bool isValid() const override;

    /*!
     * \brief sets the validation status of this FlightEntry to all items valid
     */
    void forceValid() { validationState.validateAll(); }

    /*!
     * \brief return a QStringList of invalid mandatory fields.
     *
     * \details The list is used in the GUI, so it contains user-facing translatable display names
     * instead of the database field names
     */
    QStringList invalidFields() const;

    /*!
     * \brief returns a reference to the ValidationState contained in the FlightEntry
     */
    const ValidationState& getValidationState() const;

    /*!
     * \brief returns the name of the flights table in the database
     */
    const QString getTableName() const override;

    /*!
     * \brief returns a String representation of the key data of this flight
     */
    const QString getFlightSummary() const;

    const static inline QString ROWID          = QStringLiteral("flight_id");
    const static inline QString DOFT           = QStringLiteral("doft");
    const static inline QString DEPT           = QStringLiteral("dept");
    const static inline QString DEST           = QStringLiteral("dest");
    const static inline QString TOFB           = QStringLiteral("tofb");
    const static inline QString TONB           = QStringLiteral("tonb");
    const static inline QString PIC            = QStringLiteral("pic");
    const static inline QString ACFT           = QStringLiteral("acft");
    const static inline QString TBLK           = QStringLiteral("tblk");
    const static inline QString TSPSE          = QStringLiteral("tSPSE");
    const static inline QString TSPME          = QStringLiteral("tSPME");
    const static inline QString TMP            = QStringLiteral("tMP");
    const static inline QString TNIGHT         = QStringLiteral("tNIGHT");
    const static inline QString TIFR           = QStringLiteral("tIFR");
    const static inline QString TPIC           = QStringLiteral("tPIC");
    const static inline QString TPICUS         = QStringLiteral("tPICUS");
    const static inline QString TSIC           = QStringLiteral("tSIC");
    const static inline QString TDUAL          = QStringLiteral("tDUAL");
    const static inline QString TFI            = QStringLiteral("tFI");
    const static inline QString TSIM           = QStringLiteral("tSIM");
    const static inline QString PILOTFLYING    = QStringLiteral("pilotFlying");
    const static inline QString TODAY          = QStringLiteral("toDay");
    const static inline QString TONIGHT        = QStringLiteral("toNight");
    const static inline QString LDGDAY         = QStringLiteral("ldgDay");
    const static inline QString LDGNIGHT       = QStringLiteral("ldgNight");
    const static inline QString AUTOLAND       = QStringLiteral("autoland");
    const static inline QString SECONDPILOT    = QStringLiteral("secondPilot");
    const static inline QString THIRDPILOT     = QStringLiteral("thirdPilot");
    const static inline QString APPROACHTYPE   = QStringLiteral("approachType");
    const static inline QString FLIGHTNUMBER   = QStringLiteral("flightNumber");
    const static inline QString REMARKS        = QStringLiteral("remarks");

    // Setters
    bool setDate(const QString& input, const OPL::DateTimeFormat &format);
    bool setDeparture(const QString &input);
    bool setDestination(const QString &input);
    bool setTimeOffBlocks(const QString &input, const OPL::DateTimeFormat &format);
    bool setTimeOnBlocks(const QString &input, const OPL::DateTimeFormat &format);
    bool setFirstPilot(const QString &input);
    bool setRegistration(const QString &input);
    // setBlockTime not public, calculated internally
    /*!
     * \brief determines time on aircraft category (SP/MP and SE/ME)
     * \return success if aircraft data is available
     */
    bool setAircraftCategoryTimes();
    /*!
     * \return Set the night time. Input is minutes
     */
    void setNightTime(const int nightMinutes);
    /*!
     * \brief setFunctionTimes i.e. PIC/PICUS/SIC etc. - requires block time to be known
     * \param function
     * \return false if block time cannot be determined
     */
    void setIfrTime(const int ifrMinutes);
    bool setFunctionTimes(const FlightEntry::PilotFunction function);
    void setIsPilotFlying(const bool &isPilotFlying);
    void setTakeOffCount(const int count, const bool isDay);
    void setLandingCount(const int count, const bool isDay);
    bool setSecondPilot(const QString &input);
    bool setThirdPilot(const QString &input);
    void setApproachType(const QString &approach);
    void setFlightNumber(const QString &input);
    void setRemarks(const QString &input);


    // getters
    /*!
     * \brief returns the Date of Flight in Qt::ISODATE format
     */
    const QString getDate() const;
    /*!
     * \brief returns the Departure Airport ICAO code
     */
    const QString getDeparture() const;
    /*!
     * \brief returns the Destination Airport ICAO code
     */
    const QString getDestination() const;
    /*!
     * \brief returns the time off blocks in minutes after midnight
     */
    const int getTimeOffBlocks() const;
    /*!
     * \brief returns the time on blocks in minutes after midnight
     */
    const int getTimeOnBlocks() const;
    /*!
     * \brief returns the Block Time in minutes
     */
    const int getBlockTime() const;
    /*!
     * \brief returns the rowID of the first pilot (pilots table) of this flight
     */
    const int getFirstPilotId() const;
    /*!
     * \brief returns the rowID of the second pilot (pilots table) of this flight
     */
    const int getSecondPilotId() const;
    /*!
     * \brief returns the rowID of the third pilot (pilots table) of this flight
     */
    const int getThirdPilotId() const;
    /*!
     * \brief returns the rowID of the aircraft (tails table) used during this flight
     */
    const int getRegistrationId() const;
    /*!
     * \brief returns the amount of minutes of night time during this flight
     */
    const int getNightTime() const;
    /*!
     * \brief returns the amount of minutes of IFR time during this flight
     */
    const int getIfrTime() const;
    /*!
     * \brief returns the amount of time per pilot function of this flight
     */
    const QMap<PilotFunction, int> getFunctionTimes() const;
    /*!
     * \brief true if the logbook owner was pilot flying on this flight
     */
    const bool getIsPilotFlying() const;
    /*!
     * \brief returns the number of take offs performed during this flight
     */
    const int getTakeOffCount() const;
    /*!
     * \brief returns the number of landings performed during this flight
     */
    const int getLandingCount() const;
    /*!
     * \brief returns the approach type
     */
    const QString getApproachType() const;
    /*!
     * \brief returns the remarks field for this flight
     */
    const QString getRemarks() const;
    /*!
     * \brief returns the flight number of this flight
     */
    const QString getFlightNumber() const;
private:
    ValidationState validationState;
    static constexpr int NEW_ENTRY = 0;
    static constexpr int MINUTES_PER_DAY = 24*60;
    static const inline QString TABLE_NAME = QStringLiteral("flights");

    static inline bool isValidAirport(const QString &input) {
        return DBCache->getAirportsMapICAO().key(input) != 0;
    };

    void setBlockTime();


};
} // namespace OPL
#endif // FLIGHTENTRY_H
