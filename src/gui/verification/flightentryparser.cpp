#include "flightentryparser.h"
#include "src/classes/time.h"
#include "src/database/database.h"
#include "src/database/databasecache.h"
#include "src/functions/calc.h"

namespace OPL {
using namespace OPL;

bool FlightEntryParser::isValid() const
{
    return invalidFields().isEmpty();
}

QStringList FlightEntryParser::invalidFields() const
{
    // verify mandatory fields, add invalid Items to a List
    QStringList invalidItems;

    for(const auto &field : *FlightEntry::getMandatoryFields()) {
        if(m_entryData.value(field).toInt() < 0) {
            invalidItems.append(validationItemsDisplayNames.value(field));
        }
    }

    return invalidItems;
}

QString FlightEntryParser::getFlightSummary() const
{
    return FlightEntry(m_entryData).getFlightSummary();
}

bool FlightEntryParser::setDate(const QDate &date)
{
    if(date.isValid()) {
        m_entryData.insert(FlightEntry::DOFT, date.toJulianDay());
        return true;
    }
    return false;
}

bool FlightEntryParser::setDeparture(const QString &input)
{
    if(DBCache->getAirportsMapICAO().key(input) != 0) {
        m_entryData.insert(FlightEntry::DEPT, input);
        return true;
    }
    return false;
}

bool FlightEntryParser::setDestination(const QString &input)
{
    if(DBCache->getAirportsMapICAO().key(input) != 0) {
        m_entryData.insert(FlightEntry::DEST, input);
        return true;
    }
    return false;
}

bool FlightEntryParser::setTimeOffBlocks(const QString &input, const OPL::DateTimeFormat &format)
{
    const Time time = Time::fromString(input, format);
    if( time.isValidTimeOfDay()) {
        m_entryData.insert(FlightEntry::TOFB, time.toMinutes());
        setBlockTime();
        return true;
    }

    return false;
}

bool FlightEntryParser::setTimeOnBlocks(const QString &input, const OPL::DateTimeFormat &format)
{
    const Time time = Time::fromString(input, format);
    if( time.isValidTimeOfDay()) {
        m_entryData.insert(FlightEntry::TONB, time.toMinutes());
        setBlockTime();
        return true;
    }

    return false;

}

bool FlightEntryParser::setFirstPilot(const QString &input)
{
    const int pilotId = DBCache->getPilotNamesMap().key(input);
    if(pilotId == 0) {
        m_entryData.insert(FlightEntry::PIC, QVariant(QMetaType(QMetaType::Int)));
        return false;
    }

    m_entryData.insert(FlightEntry::PIC, pilotId);
    return true;
}

bool FlightEntryParser::setSecondPilot(const QString &input)
{
    const int pilotId = DBCache->getPilotNamesMap().key(input);
    if(pilotId == 0) {
        m_entryData.insert(FlightEntry::SECONDPILOT, QVariant(QMetaType(QMetaType::Int)));
        return false;
    }

    m_entryData.insert(FlightEntry::SECONDPILOT, pilotId);
    return true;
}

bool FlightEntryParser::setThirdPilot(const QString &input)
{
    const int pilotId = DBCache->getPilotNamesMap().key(input);
    if(pilotId == 0) {
        m_entryData.insert(FlightEntry::THIRDPILOT, QVariant(QMetaType(QMetaType::Int)));
        return false;
    }

    m_entryData.insert(FlightEntry::THIRDPILOT, pilotId);
    return true;
}

bool FlightEntryParser::setRegistration(const QString &input)
{
    const int tailId = DBCache->getTailsMap().key(input);
    if(tailId == 0) {
        return false;
    }

    m_entryData.insert(FlightEntry::ACFT, tailId);
    return true;
}

void FlightEntryParser::setRemarks(const QString &input)
{
    m_entryData.insert(FlightEntry::REMARKS, input);
}

bool FlightEntryParser::setNightValues(const int nightAngle, const int toCount, const int ldgCount)
{
    // collect the required values and make sure they are valid
    bool allValid = true;

    const int tofb = m_entryData.value(FlightEntry::TOFB).toInt();
    allValid &= tofb > 0;

    const int tblk = m_entryData.value(FlightEntry::TBLK).toInt();
    allValid &= tblk > 0;

    const QString dept = m_entryData.value(FlightEntry::DEPT).toString();
    allValid &= !dept.isEmpty();

    const QString dest = m_entryData.value(FlightEntry::DEST).toString();
    allValid &= !dest.isEmpty();

    const QDate doft = QDate::fromJulianDay(m_entryData.value(FlightEntry::DOFT).toInt());
    allValid &= doft.isValid();

    if(!allValid)
        return false;

    // format and calculate

    const QTime departureTime = getTimeOffBlocks();
    const QDateTime deptDateTime(doft, departureTime);
    const auto nightData = OPL::Calc::NightTimeValues(dept, dest, deptDateTime, tblk, nightAngle);

    // Fill the calculated data
    m_entryData.insert(FlightEntry::TNIGHT, nightData.nightMinutes > 0 ? nightData.nightMinutes : QVariant());

    m_entryData.insert(FlightEntry::TODAY, nightData.takeOffNight ? QVariant() : toCount);
    m_entryData.insert(FlightEntry::TONIGHT, nightData.takeOffNight ? toCount : QVariant());

    m_entryData.insert(FlightEntry::LDGDAY, nightData.landingNight ? QVariant() : ldgCount);
    m_entryData.insert(FlightEntry::LDGNIGHT, nightData.landingNight ? ldgCount : QVariant());

    return true;
}

void FlightEntryParser::setFlightNumber(const QString &input)
{
    m_entryData.insert(FlightEntry::FLIGHTNUMBER, input);
}

void FlightEntryParser::setIsPilotFlying(const bool &isPilotFlying)
{
    m_entryData.insert(FlightEntry::PILOTFLYING, isPilotFlying);
}

void FlightEntryParser::setApproachType(const QString &approach)
{
    m_entryData.insert(FlightEntry::APPROACHTYPE, approach);
}

bool FlightEntryParser::setAircraftCategoryTimes()
{
    // check if we can retreive acft data and block time
    const int tailId = m_entryData.value(FlightEntry::ACFT).toInt();
    const int blockTime = m_entryData.value(FlightEntry::TBLK).toInt();
    if (tailId == 0 || blockTime == 0)
        return false;

    // Determine aircraft category
    const TailEntry aircraft = DB->getTailEntry(tailId);
    bool multi_pilot = aircraft.getData().value(OPL::TailEntry::MULTI_PILOT).toBool();
    bool multi_engine = aircraft.getData().value(OPL::TailEntry::MULTI_ENGINE).toBool();

    // only fill the required fields, empty out the other ones
    m_entryData.insert(OPL::FlightEntry::TMP,   multi_pilot ? QVariant(blockTime) : QVariant());
    m_entryData.insert(OPL::FlightEntry::TSPSE, (!multi_pilot && !multi_engine) ? QVariant(blockTime) : QVariant());
    m_entryData.insert(OPL::FlightEntry::TSPME, (!multi_pilot &&  multi_engine) ? QVariant(blockTime) : QVariant());

    return true;
}

const void FlightEntryParser::setBlockTime()
{
    const int tofb = m_entryData.value(FlightEntry::TOFB).toInt();
    const int tonb = m_entryData.value(FlightEntry::TONB).toInt();

    if (tofb < 0 || tonb < 0)
        return;

    // we assume no flight duration is longer than 24 hours
    int tblk = (tonb - tofb + MINUTES_PER_DAY) % MINUTES_PER_DAY;
    if (tblk < 0 || tblk >= MINUTES_PER_DAY) {
        LOG << "Invalid block time: " + QString::number(tblk);
        return;
    }

    LOG << "Block Time Calculated: " + QString::number(tblk);
    m_entryData.insert(FlightEntry::TBLK, tblk);
}

void FlightEntryParser::setNightTime(const int nightMinutes)
{
    m_entryData.insert(FlightEntry::TNIGHT, nightMinutes);
}

void FlightEntryParser::setIfrTime(const int ifrMinutes)
{
    m_entryData.insert(FlightEntry::TIFR, ifrMinutes);
}

void FlightEntryParser::setTakeOffCount(const int count, const bool isDay)
{
    isDay ? m_entryData.insert(FlightEntry::TODAY, count) : m_entryData.insert(FlightEntry::TONIGHT, count);
}

void FlightEntryParser::setLandingCount(const int count, const bool isDay)
{
    isDay ? m_entryData.insert(FlightEntry::LDGDAY, count) : m_entryData.insert(FlightEntry::LDGNIGHT, count);
}

bool FlightEntryParser::setFunctionTimes(const PilotFunction function)
{
    const int minutes = m_entryData.value(FlightEntry::TBLK).toInt();
    if( minutes <= 0)
        return false;

    for(auto it = ALL_PILOT_FUNCTIONS.cbegin(); it != ALL_PILOT_FUNCTIONS.cend(); ++it) {
        m_entryData.insert(it.value(), it.key() == function ? minutes : QVariant());
    }

    // log TFI as PIC as well
    if(function == PilotFunction::FI)
        m_entryData.insert(FlightEntry::TFI, minutes);

    return true;
}

// Getters
QString FlightEntryParser::getDeparture() const
{
    return m_entryData.value(FlightEntry::DEPT).toString();
}

QString FlightEntryParser::getDestination() const
{
    return m_entryData.value(FlightEntry::DEST).toString();
}

QDate FlightEntryParser::getDate() const
{
    return QDate::fromJulianDay(m_entryData.value(FlightEntry::DOFT).toInt());
}

QTime FlightEntryParser::getTimeOffBlocks() const
{
    const qint64 mSecs = m_entryData.value(FlightEntry::TOFB).toInt() * MILLISECONDS_PER_MINUTE;
    return QTime::fromMSecsSinceStartOfDay(mSecs);
}

QTime FlightEntryParser::getTimeOnBlocks() const
{
    const qint64 mSecs = m_entryData.value(FlightEntry::TONB).toInt() * MILLISECONDS_PER_MINUTE;
    return QTime::fromMSecsSinceStartOfDay(mSecs);
}

QTime FlightEntryParser::getBlockTime() const
{
    const qint64 mSecs = m_entryData.value(FlightEntry::TBLK).toInt() * MILLISECONDS_PER_MINUTE;
    return QTime::fromMSecsSinceStartOfDay(mSecs);
}

int FlightEntryParser::getFirstPilotId() const
{
    return m_entryData.value(FlightEntry::PIC).toInt();
}

int FlightEntryParser::getSecondPilotId() const
{
    return m_entryData.value(FlightEntry::SECONDPILOT).toInt();
}

int FlightEntryParser::getThirdPilotId() const
{
    return m_entryData.value(FlightEntry::THIRDPILOT).toInt();
}

int FlightEntryParser::getRegistrationId() const
{
    return m_entryData.value(FlightEntry::ACFT).toInt();
}

QTime FlightEntryParser::getNightTime() const
{
    const qint64 mSecs = m_entryData.value(FlightEntry::TNIGHT).toInt() * MILLISECONDS_PER_MINUTE;
    return QTime::fromMSecsSinceStartOfDay(mSecs);
}

QTime FlightEntryParser::getIfrTime() const
{
    const qint64 mSecs = m_entryData.value(FlightEntry::TIFR).toInt() * MILLISECONDS_PER_MINUTE;
    return QTime::fromMSecsSinceStartOfDay(mSecs);
}

QMap<PilotFunction, int> FlightEntryParser::getFunctionTimes() const
{
    QMap<PilotFunction, int> ret;
    for(auto it = ALL_PILOT_FUNCTIONS.cbegin(); it != ALL_PILOT_FUNCTIONS.cend(); ++it) {
        ret.insert(it.key(), m_entryData.value(it.value()).toInt());
    }
    return ret;
}

PilotFunction FlightEntryParser::getFunction() const
{
    PilotFunction function = PilotFunction::PIC;

    for(auto it = ALL_PILOT_FUNCTIONS.cbegin(); it != ALL_PILOT_FUNCTIONS.cend(); ++it) {
        if(m_entryData.value(it.value()).toInt() > 0)
            function = it.key();
    }

    return function;
}

bool FlightEntryParser::getIsPilotFlying() const
{
    return m_entryData.value(FlightEntry::PILOTFLYING).toBool();
}

int FlightEntryParser::getTakeOffCount() const
{
    return m_entryData.value(FlightEntry::TODAY).toInt() + m_entryData.value(FlightEntry::TONIGHT).toInt();
}

int FlightEntryParser::getLandingCount() const
{
    return m_entryData.value(FlightEntry::LDGDAY).toInt() + m_entryData.value(FlightEntry::LDGNIGHT).toInt();
}

QString FlightEntryParser::getApproachType() const
{
    return m_entryData.value(FlightEntry::APPROACHTYPE).toString();
}

QString FlightEntryParser::getRemarks() const
{
    return m_entryData.value(FlightEntry::REMARKS).toString();
}

QString FlightEntryParser::getFlightNumber() const
{
    return m_entryData.value(FlightEntry::FLIGHTNUMBER).toString();
}

} // namespace OPL
