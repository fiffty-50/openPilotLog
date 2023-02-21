#include "randomgenerator.h"
#include "src/database/database.h"
#include "src/functions/calc.h"
#include "src/opl.h"
#include "src/functions/time.h"

namespace OPL {

RandomGenerator::RandomGenerator(bool safe_mode) :
    safeMode(safe_mode)
{
    m_numberOfAirports = DB->getLastEntry(OPL::DbTable::Airports);
    m_numberOfTails = DB->getLastEntry(OPL::DbTable::Tails);
    m_numberOfPilots = DB->getLastEntry(OPL::DbTable::Pilots);
}

const FlightEntry RandomGenerator::randomFlight()
{
    const QDateTime dept_dt = randomDateTime();
    const QDateTime dest_dt = dept_dt.addSecs(QRandomGenerator::global()->bounded(900, 50000));

    const QString doft = dept_dt.date().toString(Qt::ISODate);
    int tofb = OPL::Time::toMinutes(dept_dt.time());
    int tonb = OPL::Time::toMinutes(dest_dt.time());

    int pic = randomPilot();
    int acft = randomTail();

    const QString dept = randomAirport();
    const QString dest = randomAirport();

    int tblk = OPL::Time::blockMinutes(dept_dt.time(), dest_dt.time());
    int tNight = OPL::Calc::calculateNightTime(dept, dest, dept_dt, tblk, 6);

    auto flt_data = OPL::RowData_T();
    flt_data.insert(OPL::Db::FLIGHTS_DOFT, doft);
    flt_data.insert(OPL::Db::FLIGHTS_DEPT, dept);
    flt_data.insert(OPL::Db::FLIGHTS_DEST, dest);
    flt_data.insert(OPL::Db::FLIGHTS_PIC, pic);
    flt_data.insert(OPL::Db::FLIGHTS_ACFT, acft);
    flt_data.insert(OPL::Db::FLIGHTS_TOFB, tofb);
    flt_data.insert(OPL::Db::FLIGHTS_TONB, tonb);
    flt_data.insert(OPL::Db::FLIGHTS_TBLK, tblk);

    if (tNight > 0) flt_data.insert(OPL::Db::FLIGHTS_TNIGHT, tNight);

    bool pf = randomBool(); // Pilot Flying

    // Take-Off and Landing
    if (pf) {
        flt_data.insert(OPL::Db::FLIGHTS_PILOTFLYING, 1);
        if (OPL::Calc::isNight(dept, dept_dt, 6))
            flt_data.insert(OPL::Db::FLIGHTS_TONIGHT, 1);
        else
            flt_data.insert(OPL::Db::FLIGHTS_TODAY, 1);
        if (OPL::Calc::isNight(dest, dest_dt, 6))
            flt_data.insert(OPL::Db::FLIGHTS_LDGNIGHT, 1);
        else
            flt_data.insert(OPL::Db::FLIGHTS_LDGDAY, 1);
    }

    int function;
    if (pic == 1) {
        flt_data.insert(OPL::Db::FLIGHTS_TPIC, tblk);
        flt_data.insert(OPL::Db::FLIGHTS_SECONDPILOT, randomPilot());
    } else {
        function = QRandomGenerator::global()->bounded(1,4);
        flt_data.insert(OPL::Db::FLIGHTS_SECONDPILOT, 1);
        flt_data.insert(m_function_times[function], tblk);
    }

    return OPL::FlightEntry(flt_data);
}

const QTime RandomGenerator::randomTime()
{
    int h = QRandomGenerator::global()->bounded(0, 23);
    int m = QRandomGenerator::global()->bounded(0, 59);
    return QTime(h,m);
}

const QDate RandomGenerator::randomDate()
{
    int year = QRandomGenerator::global()->bounded(2000, 2021);
    int month = QRandomGenerator::global()->bounded(1,12);
    int day = QRandomGenerator::global()->bounded(1, 28);
    return QDate(year, month, day);
}

const QDateTime RandomGenerator::randomDateTime()
{
    int year = QRandomGenerator::global()->bounded(2000, 2021);
    int month = QRandomGenerator::global()->bounded(1,12);
    int day = QRandomGenerator::global()->bounded(1, 28);
    int hour = QRandomGenerator::global()->bounded(0, 23);
    int minute = QRandomGenerator::global()->bounded(0,59);

    return QDateTime(QDate(year, month, day), QTime(hour, minute));
}

const QString RandomGenerator::randomAirport()
{
    return DB->getAirportEntry(QRandomGenerator::global()->bounded(1, m_numberOfAirports)).icao();
}

const int RandomGenerator::randomPilot()
{
    if (!safeMode)
        return QRandomGenerator::global()->bounded(1, m_numberOfPilots);

    // verify entry exists before returning
    int pilot;
    do {
        pilot = QRandomGenerator::global()->bounded(1, m_numberOfPilots);
    } while (!DB->exists(Row(OPL::DbTable::Pilots, pilot)));
    return pilot;
}

const int RandomGenerator::randomTail()
{
    if (!safeMode)
        return QRandomGenerator::global()->bounded(1, m_numberOfTails);

    // verify entry exists before returning
    int acft;
    do {
        acft = QRandomGenerator::global()->bounded(1, m_numberOfTails);
    } while (!DB->exists(Row(DbTable::Tails, acft)));
    return acft;
}

const bool RandomGenerator::randomBool()
{
#if HAVE_ARC4RANDOM
    return arc4random() > (RAND_MAX / 2);
#else
    auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    return gen();
#endif
}

} // namespace OPL
