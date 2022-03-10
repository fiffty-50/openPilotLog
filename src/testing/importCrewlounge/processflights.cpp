#include "processflights.h"
#include <src/functions/atime.h>

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
        new_flight_data.insert(OPL::Db::FLIGHTS_FLIGHTNUMBER, row[1]);
        new_flight_data.insert(OPL::Db::FLIGHTS_DEPT, row[2]);
        new_flight_data.insert(OPL::Db::FLIGHTS_DEST, row[3]);
        new_flight_data.insert(OPL::Db::FLIGHTS_TBLK, row[6]);
        new_flight_data.insert(OPL::Db::FLIGHTS_TPIC, row[7]);
        new_flight_data.insert(OPL::Db::FLIGHTS_TSIC, row[8]);
        new_flight_data.insert(OPL::Db::FLIGHTS_TDUAL, row[9]);
        new_flight_data.insert(OPL::Db::FLIGHTS_TPICUS, row[10]);
        new_flight_data.insert(OPL::Db::FLIGHTS_TFI, row[11]);
        new_flight_data.insert(OPL::Db::FLIGHTS_TNIGHT, row[12]);
        new_flight_data.insert(OPL::Db::FLIGHTS_TODAY, row[16]);
        new_flight_data.insert(OPL::Db::FLIGHTS_TONIGHT, row[17]);
        new_flight_data.insert(OPL::Db::FLIGHTS_LDGDAY, row[18]);
        new_flight_data.insert(OPL::Db::FLIGHTS_LDGNIGHT, row[19]);
        new_flight_data.insert(OPL::Db::FLIGHTS_APPROACHTYPE, row[21]);
        new_flight_data.insert(OPL::Db::FLIGHTS_REMARKS, row[22]);

        // PF
        if (row[20] == QLatin1String("TRUE"))
            new_flight_data.insert(OPL::Db::FLIGHTS_PILOTFLYING, 1);
        else
            new_flight_data.insert(OPL::Db::FLIGHTS_PILOTFLYING, 0);

        // Convert Date and Time
        const QDate doft = QDate::fromString(row[0],QStringLiteral("dd/MM/yyyy"));
        new_flight_data.insert(OPL::Db::FLIGHTS_DOFT, doft.toString(Qt::ISODate));

        auto time_off = QTime::fromString(row[4], QStringLiteral("hh:mm"));
        if (!time_off.isValid())
            time_off = QTime::fromString(row[4], QStringLiteral("h:mm"));
        int tofb = ATime::toMinutes(time_off);
        new_flight_data.insert(OPL::Db::FLIGHTS_TOFB, tofb);

        auto time_on = QTime::fromString(row[5], QStringLiteral("hh:mm"));
        if (!time_on.isValid())
            time_on = QTime::fromString(row[5], QStringLiteral("h:mm"));

        int tonb = ATime::toMinutes(time_on);
        new_flight_data.insert(OPL::Db::FLIGHTS_TONB, tonb);

        // map pilots
        int pic = processedPilotsIds.value(row[13]);
        new_flight_data.insert(OPL::Db::FLIGHTS_PIC, pic);
        int second_pilot = processedPilotsIds.value(row[14]);
        new_flight_data.insert(OPL::Db::FLIGHTS_SECONDPILOT, second_pilot);
        int third_pilot = processedPilotsIds.value(row[15]);
        new_flight_data.insert(OPL::Db::FLIGHTS_THIRDPILOT, third_pilot);

        // map tail
        int acft = processedTailsIds.value(row[23]);
        new_flight_data.insert(OPL::Db::FLIGHTS_ACFT, acft);

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
