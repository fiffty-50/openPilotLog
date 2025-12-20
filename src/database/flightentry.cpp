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
#include "flightentry.h"
#include "src/classes/date.h"
#include "src/classes/time.h"
#include "src/database/database.h"
#include "src/database/tailentry.h"
#include "src/opl.h"

namespace OPL {

FlightEntry::FlightEntry()
    : Row(DbTable::Flights, NEW_ENTRY)
{}

FlightEntry::FlightEntry(const RowData_T &row_data)
    : Row(DbTable::Flights, NEW_ENTRY, row_data)
{}

FlightEntry::FlightEntry(int row_id, const RowData_T &row_data)
    : Row(DbTable::Flights, row_id, row_data)
{}

bool FlightEntry::isValid() const
{
    return Row::isValid() && validationState.allValid();
}

QStringList FlightEntry::invalidFields() const
{
    if(isValid())
        return {};

    return validationState.invalidItemDisplayNames();
}

const QString FlightEntry::getTableName() const
{
    return TABLE_NAME;
}

const QString FlightEntry::getFlightSummary() const
{
    using namespace OPL;
    if(!isValid())
        return QString();

    auto tableData = getData();
    QString flight_summary;
    constexpr auto space = QLatin1Char(' ');
    flight_summary.append(Date(tableData.value(FlightEntry::DOFT).toInt(), DateTimeFormat()).toString() + space);
    flight_summary.append(tableData.value(FlightEntry::DEPT).toString() + space);
    flight_summary.append(Time(tableData.value(FlightEntry::TOFB).toInt(), DateTimeFormat()).toString()
                          + space);
    flight_summary.append(Time(tableData.value(FlightEntry::TONB).toInt(), DateTimeFormat()).toString()
                          + space);
    flight_summary.append(tableData.value(FlightEntry::DEST).toString());

    return flight_summary;
}

bool FlightEntry::setDate(const QString &input, const DateTimeFormat &format)
{
    OPL::Date date(input, format);
    if(!date.isValid()) {
        validationState.invalidate(ValidationState::DOFT);
        return false;
    }

    rowData.insert(OPL::FlightEntry::DOFT, date.toString());
    validationState.validate(ValidationState::DOFT);
    return true;

}

bool FlightEntry::setDeparture(const QString &input)
{
    if(!isValidAirport(input)) {
        validationState.invalidate(ValidationState::DEPT);
        return false;
    }

    validationState.validate(ValidationState::DEPT);
    rowData.insert(OPL::FlightEntry::DEPT, input);
    return true;
}

bool FlightEntry::setDestination(const QString &input)
{
    if(!isValidAirport(input)) {
        validationState.invalidate(ValidationState::DEST);
        return false;
    }

    validationState.validate(ValidationState::DEST);
    rowData.insert(OPL::FlightEntry::DEST, input);
    return true;
}

bool FlightEntry::setTimeOffBlocks(const QString &input, const DateTimeFormat &format)
{
    const Time time = Time::fromString(input, format);
    if(! time.isValidTimeOfDay()) {
        validationState.invalidate(ValidationState::TOFB);
        return false;
    }

    validationState.validate(ValidationState::TOFB);
    rowData.insert(OPL::FlightEntry::TOFB, time.toMinutes());
    setBlockTime();
    return true;
}

bool FlightEntry::setTimeOnBlocks(const QString &input, const DateTimeFormat &format)
{
    const Time time = Time::fromString(input, format);
    if(! time.isValidTimeOfDay()) {
        validationState.invalidate(ValidationState::TONB);
        return false;
    }

    validationState.validate(ValidationState::TONB);
    rowData.insert(FlightEntry::TONB, time.toMinutes());
    setBlockTime();
    return true;

}

bool FlightEntry::setFirstPilot(const QString &input)
{
    const int pilotId = DBCache->getPilotNamesMap().key(input);
    if(pilotId == 0) {
        validationState.invalidate(ValidationState::PIC);
        return false;
    }

    validationState.validate(ValidationState::PIC);
    rowData.insert(FlightEntry::PIC, pilotId);
    return true;
}

bool FlightEntry::setRegistration(const QString &input)
{
    const int tailId = DBCache->getTailsMap().key(input);
    if(tailId == 0) {
        validationState.invalidate(ValidationState::ACFT);
        return false;
    }

    validationState.validate(ValidationState::ACFT);
    rowData.insert(FlightEntry::ACFT, tailId);
    return true;
}

bool FlightEntry::setSecondPilot(const QString &input)
{
    const int pilotId = DBCache->getPilotNamesMap().key(input);
    if(pilotId == 0) {
        return false;
    }

    rowData.insert(FlightEntry::SECONDPILOT, pilotId);
    return true;

}

bool FlightEntry::setThirdPilot(const QString &input)
{
    const int pilotId = DBCache->getPilotNamesMap().key(input);
    if(pilotId == 0) {
        return false;
    }

    rowData.insert(FlightEntry::THIRDPILOT, pilotId);
    return true;

}

void FlightEntry::setRemarks(const QString &input)
{
    rowData.insert(FlightEntry::REMARKS, input);
}

void FlightEntry::setFlightNumber(const QString &input)
{
    rowData.insert(FlightEntry::FLIGHTNUMBER, input);
}

void FlightEntry::setIsPilotFlying(const bool &isPilotFlying)
{
    rowData.insert(FlightEntry::PILOTFLYING, isPilotFlying);
}

void FlightEntry::setApproachType(const QString &approach)
{
    rowData.insert(FlightEntry::APPROACHTYPE, approach);
}

bool FlightEntry::setAircraftCategoryTimes()
{
    // check if we can retreive acft data and block time
    const int tailId = rowData.value(FlightEntry::ACFT).toInt();
    const int blockTime = rowData.value(FlightEntry::TBLK).toInt();
    if (tailId == 0 || blockTime == 0)
        return false;

    // Determine aircraft category
    const TailEntry aircraft = DB->getTailEntry(tailId);
    bool multi_pilot = aircraft.getData().value(OPL::TailEntry::MULTI_PILOT).toBool();
    bool multi_engine = aircraft.getData().value(OPL::TailEntry::MULTI_ENGINE).toBool();

    // only fill the required fields, empty out the other ones
    rowData.insert(OPL::FlightEntry::TMP,   multi_pilot ? QVariant(blockTime) : QString());
    rowData.insert(OPL::FlightEntry::TSPSE, (!multi_pilot && !multi_engine) ? QVariant(blockTime) : QString());
    rowData.insert(OPL::FlightEntry::TSPME, (!multi_pilot &&  multi_engine) ? QVariant(blockTime) : QString());

    return true;
}

void FlightEntry::setNightTime(const int nightMinutes)
{
    rowData.insert(FlightEntry::TNIGHT, nightMinutes);
}

void FlightEntry::setIfrTime(const int ifrMinutes)
{
    rowData.insert(FlightEntry::TIFR, ifrMinutes);
}

void FlightEntry::setTakeOffCount(const int count, const bool isDay)
{
    isDay ? rowData.insert(FlightEntry::TODAY, count) : rowData.insert(FlightEntry::TONIGHT, count);
}

void FlightEntry::setLandingCount(const int count, const bool isDay)
{
    isDay ? rowData.insert(FlightEntry::LDGDAY, count) : rowData.insert(FlightEntry::LDGNIGHT, count);
}

bool FlightEntry::setFunctionTimes(const PilotFunction function)
{
    const int minutes = rowData.value(FlightEntry::TBLK).toInt();
    if( minutes == 0)
        return false;

    // Map flight function to loggable DB fields
    const QHash<PilotFunction, QSet<QString>> function_map = {
        { PilotFunction::PIC,   { OPL::FlightEntry::TPIC   } },
        { PilotFunction::PICUS, { OPL::FlightEntry::TPICUS } },
        { PilotFunction::SIC,   { OPL::FlightEntry::TSIC   } },
        { PilotFunction::DUAL,  { OPL::FlightEntry::TDUAL  } },
        { PilotFunction::FI,    { OPL::FlightEntry::TFI,
                              OPL::FlightEntry::TPIC  } } // FI logs PIC too
    };

    const QSet<QString> all_entries = {
        OPL::FlightEntry::TPIC,
        OPL::FlightEntry::TPICUS,
        OPL::FlightEntry::TSIC,
        OPL::FlightEntry::TDUAL,
        OPL::FlightEntry::TFI
    };

    const QSet<QString>& active = function_map.value(function);

    for (const QString& entry : all_entries) {
        rowData.insert(entry, active.contains(entry) ? minutes : QVariant());
    }
    return true;
}

const QString FlightEntry::getDeparture() const
{
    return rowData.value(FlightEntry::DEPT).toString();
}

const QString FlightEntry::getDestination() const
{
    return rowData.value(FlightEntry::DEST).toString();
}

const QString FlightEntry::getDate() const
{
    return rowData.value(FlightEntry::DOFT).toString();
}

const int FlightEntry::getTimeOffBlocks() const
{
    return rowData.value(FlightEntry::TOFB).toInt();
}

const int FlightEntry::getTimeOnBlocks() const
{
    return rowData.value(FlightEntry::TONB).toInt();
}

const int FlightEntry::getBlockTime() const
{
    return rowData.value(FlightEntry::TBLK).toInt();
}

const int FlightEntry::getFirstPilotId() const
{
    return rowData.value(FlightEntry::PIC).toInt();
}

const int FlightEntry::getSecondPilotId() const
{
    return rowData.value(FlightEntry::SECONDPILOT).toInt();
}

const int FlightEntry::getThirdPilotId() const
{
    return rowData.value(FlightEntry::THIRDPILOT).toInt();
}

const int FlightEntry::getRegistrationId() const
{
    return rowData.value(FlightEntry::ACFT).toInt();
}

const int FlightEntry::getNightTime() const
{
    return rowData.value(FlightEntry::TNIGHT).toInt();
}

const int FlightEntry::getIfrTime() const
{
    return rowData.value(FlightEntry::TIFR).toInt();
}

const bool FlightEntry::getIsPilotFlying() const
{
    return rowData.value(FlightEntry::PILOTFLYING).toBool();
}

const int FlightEntry::getTakeOffCount() const
{
    return rowData.value(FlightEntry::TODAY).toInt() + rowData.value(FlightEntry::TONIGHT).toInt();
}

const int FlightEntry::getLandingCount() const
{
    return rowData.value(FlightEntry::LDGDAY).toInt() + rowData.value(FlightEntry::LDGNIGHT).toInt();
}

const QString FlightEntry::getApproachType() const
{
    return rowData.value(FlightEntry::APPROACHTYPE).toString();
}

const QString FlightEntry::getRemarks() const
{
    return rowData.value(FlightEntry::REMARKS).toString();
}

const QString FlightEntry::getFlightNumber() const
{
    return rowData.value(FlightEntry::FLIGHTNUMBER).toString();
}

const QMap<FlightEntry::PilotFunction, int> FlightEntry::getFunctionTimes() const
{
    Q_UNIMPLEMENTED();
    return {};
}

void FlightEntry::setBlockTime()
{
    if(!validationState.timesValid())
        return;

    DEB << "Calculating Block Time... ";
    const int tofb = rowData.value(FlightEntry::TOFB).toInt();
    const int tonb = rowData.value(FlightEntry::TONB).toInt();
    DEB << "Calculating Block Time: " + QString::number(tofb) + '-' + QString::number(tonb);

    // we assume no flight duration is longer than 24 hours
    int tblk = (tonb - tofb + MINUTES_PER_DAY) % MINUTES_PER_DAY;
    if (tblk < 0 || tblk >= MINUTES_PER_DAY) {
        LOG << "Invalid block time: " + QString::number(tblk);
        return;
    }

    LOG << "Block Time Calculated: " + QString::number(tblk);
    rowData.insert(FlightEntry::TBLK, tblk);
}

const ValidationState &FlightEntry::getValidationState() const
{
    return validationState;
}



} // namespace OPL
