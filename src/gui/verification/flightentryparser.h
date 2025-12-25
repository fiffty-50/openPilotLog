#ifndef FLIGHTENTRYPARSER_H
#define FLIGHTENTRYPARSER_H

#include "src/database/flightentry.h"

namespace OPL {

/*!
 * \brief The FlightEntryParser class provides an interface from user input
 * to database values
 * \details Most values in the database are not human readable format. User input
 * needs to be verified and converted to the database format before it can be
 * submitted. The FlightEntryParser class takes User input from the UI, validates
 * and converts it. It also converts database values into user-facing formats.
 *
 * As such, this class is mostly a wrapper with setters and getters for the raw
 * OPL::FlightEntry data.
 */
class FlightEntryParser
{
public:
    FlightEntryParser()
    {
        m_entryData = OPL::RowData_T();
        m_rowId = NEW_ENTRY;
    }
    explicit FlightEntryParser(OPL::FlightEntry entry)
    {
        m_entryData = entry.getData();
        m_rowId = entry.getRowId();
    }
    static int timeToMinutes(const QTime &time) { return time.hour() * 60 + time.minute(); }
    static QTime minutesToTime(const int minutes) {
        return QTime::fromMSecsSinceStartOfDay(minutes * MILLISECONDS_PER_MINUTE); }



    /*!
     * \brief true if the entry can be submitted to the database
     * \details A FlightEntry is considered valid if all its mandatory
     * elements are not null and all elements that have foreign key
     * constraints in the database are valid.
     */
    bool isValid() const;
    OPL::FlightEntry getFlightEntry() const { return OPL::FlightEntry(m_rowId, m_entryData); }
    QStringList invalidFields() const;
    QString getFlightSummary() const;


    // Setters
    bool setDate(const QDate &date);
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
    bool setFunctionTimes(const OPL::PilotFunction function);
    void setIsPilotFlying(const bool &isPilotFlying);
    void setTakeOffCount(const int count, const bool isDay);
    void setLandingCount(const int count, const bool isDay);
    bool setSecondPilot(const QString &input);
    bool setThirdPilot(const QString &input);
    void setApproachType(const QString &approach);
    void setFlightNumber(const QString &input);
    void setRemarks(const QString &input);

    /*!
     * \brief setNightValues - Calculate and set Night Time and TO/LDG
     * \return false if data is missing and calculation cannot be done
     * \details This function can be used instead of manuall setting these
     * values. It replaces the use of setTakeOffCount, setLandingCount and
     * setNightMinutes.
     */
    bool setNightValues(const int nightAngle, const int toCount, const int ldgCount);


    // getters
    /*!
     * \brief returns the Date of Flight in Qt::ISODATE format
     */
    QDate getDate() const;
    /*!
     * \brief returns the Departure Airport ICAO code
     */
    QString getDeparture() const;
    /*!
     * \brief returns the Destination Airport ICAO code
     */
    QString getDestination() const;
    /*!
     * \brief returns the time off blocks
     */
    QTime getTimeOffBlocks() const;
    /*!
     * \brief returns the time on blocks
     */
    QTime getTimeOnBlocks() const;
    /*!
     * \brief returns the Block Time
     */
    QTime getBlockTime() const;
    /*!
     * \brief returns the rowID of the first pilot (pilots table) of this flight
     */
    int getFirstPilotId() const;
    /*!
     * \brief returns the rowID of the second pilot (pilots table) of this flight
     */
    int getSecondPilotId() const;
    /*!
     * \brief returns the rowID of the third pilot (pilots table) of this flight
     */
    int getThirdPilotId() const;
    /*!
     * \brief returns the rowID of the aircraft (tails table) used during this flight
     */
    int getRegistrationId() const;
    /*!
     * \brief returns the amount of minutes of night time during this flight
     */
    QTime getNightTime() const;
    /*!
     * \brief returns the amount of minutes of IFR time during this flight
     */
    QTime getIfrTime() const;
    /*!
     * \brief returns the amount of time per pilot function of this flight
     */
    QMap<OPL::PilotFunction, int> getFunctionTimes() const;

    /*!
     * \brief returns the OPL::PilotFunction recorded in the entry
     * \note The highest function is returned
     */
    OPL::PilotFunction getFunction() const;
    /*!
     * \brief true if the logbook owner was pilot flying on this flight
     */
    bool getIsPilotFlying() const;
    /*!
     * \brief returns the number of take offs performed during this flight
     */
    int getTakeOffCount() const;
    /*!
     * \brief returns the number of landings performed during this flight
     */
    int getLandingCount() const;
    /*!
     * \brief returns the approach type
     */
    QString getApproachType() const;
    /*!
     * \brief returns the remarks field for this flight
     */
    QString getRemarks() const;
    /*!
     * \brief returns the flight number of this flight
     */
    QString getFlightNumber() const;

private:
    OPL::RowData_T m_entryData;
    int m_rowId;

    const bool nightDataValid() const;
    const void setBlockTime();
    static const inline QMap<QStringView, QString> validationItemsDisplayNames = {
        {OPL::FlightEntry::DOFT, QObject::tr("Date of Flight")},
        {OPL::FlightEntry::DEPT, QObject::tr("Departure Airport")},
        {OPL::FlightEntry::DEST, QObject::tr("Destination Airport")},
        {OPL::FlightEntry::TOFB, QObject::tr("Time Off Blocks")},
        {OPL::FlightEntry::TONB, QObject::tr("Time On Blocks")},
        {OPL::FlightEntry::TBLK, QObject::tr("Block Time")},
        {OPL::FlightEntry::PIC,  QObject::tr("PIC Name")},
        {OPL::FlightEntry::ACFT, QObject::tr("Aircraft Registration")},
        };

    // Map flight function to loggable DB fields
    static inline const QHash<OPL::PilotFunction, QSet<QStringView>> pilotFunctionMap = {
        { OPL::PilotFunction::PIC,   { OPL::FlightEntry::TPIC   } },
        { OPL::PilotFunction::PICUS, { OPL::FlightEntry::TPICUS } },
        { OPL::PilotFunction::SIC,   { OPL::FlightEntry::TSIC   } },
        { OPL::PilotFunction::DUAL,  { OPL::FlightEntry::TDUAL  } },
        { OPL::PilotFunction::FI,    { OPL::FlightEntry::TFI,
                                       OPL::FlightEntry::TPIC  } } // FI logs PIC too
    };

    /*!
     * \brief ALL_PILOT_FUNCTIONS contains all values of OPL::PilotFunction
     */
    static inline const QMap<PilotFunction, QString > ALL_PILOT_FUNCTIONS = {
        { PilotFunction::PIC, 	QString(FlightEntry::TPIC) },
        { PilotFunction::PICUS, QString(FlightEntry::TPICUS) },
        { PilotFunction::SIC, 	QString(FlightEntry::TSIC)  },
        { PilotFunction::DUAL, 	QString(FlightEntry::TDUAL) },
        { PilotFunction::FI, 	QString(FlightEntry::TFI)},
        };

    static constexpr int MINUTES_PER_DAY = 60 * 24;
    static constexpr int MILLISECONDS_PER_MINUTE = 60 * 1000;
    static constexpr int NEW_ENTRY = 0;
};

#endif // FLIGHTENTRYPARSER_H

} // namespace OPL
