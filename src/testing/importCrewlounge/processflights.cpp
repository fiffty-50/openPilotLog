#include "processflights.h"
#include "src/classes/time.h"
#include "src/database/flightentry.h"

void ProcessFlights::parseRawData()
{
    // doft, flightNumber, dept, dest, tofb, tonb, tblk, tPIC, tSIC, tDUAL, tPICUS, tFI, tNight, pic, secondPilot, thirdPilot   toDN,ldDN   pilotFlying, appType, remarks,  acftReg
    int relevant_cols[24] = {0,3,5,7,9,11,17,19,20,21,22,23,25,38,42,46,53,54,55,56,58,60,64,79};
    QStringList row_data;
    for (const auto &row : qAsConst(rawData)) {
        for (const auto &col : relevant_cols) {
            row_data.append(row[col]);
        }
        rawFlightData.append(row_data);
        row_data.clear();
    }
    DEB << "Flight Info #1742:" << rawFlightData[1742];
}

void ProcessFlights::processParsedData()
{
    QHash<QString, QVariant> new_flight_data;
    int flight_id = 1;

    for (const auto &row : qAsConst(rawFlightData)) {
        // insert values that don't require editing
        new_flight_data.insert(OPL::FlightEntry::FLIGHTNUMBER, row[1]);
        new_flight_data.insert(OPL::FlightEntry::DEPT, row[2]);
        new_flight_data.insert(OPL::FlightEntry::DEST, row[3]);
        new_flight_data.insert(OPL::FlightEntry::TBLK, row[6]);
        new_flight_data.insert(OPL::FlightEntry::TPIC, row[7]);
        new_flight_data.insert(OPL::FlightEntry::TSIC, row[8]);
        new_flight_data.insert(OPL::FlightEntry::TDUAL, row[9]);
        new_flight_data.insert(OPL::FlightEntry::TPICUS, row[10]);
        new_flight_data.insert(OPL::FlightEntry::TFI, row[11]);
        new_flight_data.insert(OPL::FlightEntry::TNIGHT, row[12]);
        new_flight_data.insert(OPL::FlightEntry::TODAY, row[16]);
        new_flight_data.insert(OPL::FlightEntry::TONIGHT, row[17]);
        new_flight_data.insert(OPL::FlightEntry::LDGDAY, row[18]);
        new_flight_data.insert(OPL::FlightEntry::LDGNIGHT, row[19]);
        new_flight_data.insert(OPL::FlightEntry::APPROACHTYPE, row[21]);
        new_flight_data.insert(OPL::FlightEntry::REMARKS, row[22]);

        // PF
        if (row[20] == QLatin1String("TRUE"))
            new_flight_data.insert(OPL::FlightEntry::PILOTFLYING, 1);
        else
            new_flight_data.insert(OPL::FlightEntry::PILOTFLYING, 0);

        // Convert Date and Time
        const QDate doft = QDate::fromString(row[0],QStringLiteral("dd/MM/yyyy"));
        new_flight_data.insert(OPL::FlightEntry::DOFT, doft.toString(Qt::ISODate));

        auto time_off = QTime::fromString(row[4], QStringLiteral("hh:mm"));
        if (!time_off.isValid())
            time_off = QTime::fromString(row[4], QStringLiteral("h:mm"));
        int tofb = OPL::Time::fromString(time_off.toString(), OPL::DateTimeFormat()).toMinutes();
        new_flight_data.insert(OPL::FlightEntry::TOFB, tofb);

        auto time_on = QTime::fromString(row[5], QStringLiteral("hh:mm"));
        if (!time_on.isValid())
            time_on = QTime::fromString(row[5], QStringLiteral("h:mm"));

        int tonb = OPL::Time::fromString(time_on.toString(), OPL::DateTimeFormat()).toMinutes();
        new_flight_data.insert(OPL::FlightEntry::TONB, tonb);

        // map pilots
        int pic = processedPilotsIds.value(row[13]);
        new_flight_data.insert(OPL::FlightEntry::PIC, pic);
        int second_pilot = processedPilotsIds.value(row[14]);
        new_flight_data.insert(OPL::FlightEntry::SECONDPILOT, second_pilot);
        int third_pilot = processedPilotsIds.value(row[15]);
        new_flight_data.insert(OPL::FlightEntry::THIRDPILOT, third_pilot);

        // map tail
        int acft = processedTailsIds.value(row[23]);
        new_flight_data.insert(OPL::FlightEntry::ACFT, acft);

        // set id, fix opl to include alias
        new_flight_data.insert(QStringLiteral("flight_id"), flight_id);
        processedFlights.append(new_flight_data);
        new_flight_data.clear();
        flight_id ++;
    }
}

QVector<QHash<QString, QVariant> > ProcessFlights::getProcessedFlights() const
{
    return processedFlights;
}
