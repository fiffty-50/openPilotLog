#include "processpilots.h"
#include "src/database/pilotentry.h"

void ProcessPilots::parseRawData()
{
    const QVector<int> cols_pilot1 = {37, 38, 39, 40}; //empId, name, phone, email
    const QVector<int> cols_pilot2 = {41, 42, 43, 44};
    const QVector<int> cols_pilot3 = {45, 46, 47, 48};
    const QVector<QVector<int>> pilot_cols = {
        cols_pilot1,
        cols_pilot2,
        cols_pilot3
    };

    QVector<QStringList> unique_pilots;
    QStringList pilot_data = {
        QString(),
        QStringLiteral("SELF"),
        QString(),
        QString()
    };
    unique_pilots.append(pilot_data);
    int unique_pilot_id = 1;
    processedPilotsIds.insert(pilot_data[1], unique_pilot_id);
    rawPilotsAndIds.append({pilot_data, unique_pilot_id});
    pilot_data.clear();
    unique_pilot_id ++;


    for (const auto &row : std::as_const(rawData)) {
        for (const auto &col_array : pilot_cols) {
            for (const auto &col : col_array) {
                pilot_data.append(row[col]);
            }
            if (!unique_pilots.contains(pilot_data) && !pilot_data.contains(QLatin1String("SELF"))) {
                unique_pilots.append(pilot_data);
                processedPilotsIds.insert(pilot_data[1], unique_pilot_id);
                rawPilotsAndIds.append({pilot_data, unique_pilot_id});
                unique_pilot_id ++;
            }
            pilot_data.clear();
        }
    }
}

void ProcessPilots::processParsedData()
{
    for (const auto &pair : std::as_const(rawPilotsAndIds)) {
        //DEB << "ID:" << pair.second << "Details:" << pair.first;
        OPL::RowData_T new_pilot_data;

        // process name [1]
        auto temp_list = pair.first[1].split(QLatin1Char(' '));
        if (!temp_list.isEmpty()) {
            new_pilot_data.insert(OPL::PilotEntry::LASTNAME, temp_list.first());
            temp_list.pop_front();

            if (!temp_list.isEmpty())
                new_pilot_data.insert(OPL::PilotEntry::FIRSTNAME, temp_list.join(QLatin1Char(' ')));
        } else {
            new_pilot_data.insert(OPL::PilotEntry::LASTNAME, QStringLiteral("UNKNOWN"));
        }

        // add additional data
        new_pilot_data.insert(OPL::PilotEntry::EMPLOYEEID, pair.first[0]);
        new_pilot_data.insert(OPL::PilotEntry::PHONE, pair.first[2]);
        new_pilot_data.insert(OPL::PilotEntry::EMAIL, pair.first[3]);

        // add pilot_id (workaround with literal until OPL::Db is updated)
        new_pilot_data.insert(QStringLiteral("pilot_id"), pair.second);

        processedPilotHashes.insert(pair.first[1], new_pilot_data);
        processedPilotsIds.insert(pair.first[1], pair.second);
    }
}

QHash<QString, OPL::RowData_T>ProcessPilots::getProcessedPilotMaps() const
{
    return processedPilotHashes;
}

QHash<QString, int> ProcessPilots::getProcessedPilotsIds() const
{
    return processedPilotsIds;
}
