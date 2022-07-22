#include "importcrewlounge.h"
#include "src/database/database.h"
#include "src/opl.h"
#include "src/database/row.h"
#include "src/testing/importCrewlounge/processpilots.h"
#include "src/testing/importCrewlounge/processaircraft.h"
#include "src/testing/importCrewlounge/processflights.h"
#include "src/functions/areadcsv.h"

namespace ImportCrewlounge
{

void exec(const QString &csv_file_path)
{
    // Inhibit HomeWindow Updating
    QSignalBlocker blocker(DB);

    // Prepare database and set up exclusive transaction for mass commit
    QSqlQuery q;
    q.prepare(QStringLiteral("BEGIN EXCLUSIVE TRANSACTION"));
    q.exec();

    // Read from CSV and remove first line (headers)
    auto raw_csv_data = aReadCsvAsRows(csv_file_path);
    raw_csv_data.removeFirst();

    // Process Pilots
    auto proc_pilots = ProcessPilots(raw_csv_data);
    proc_pilots.init();
    const auto p_maps = proc_pilots.getProcessedPilotMaps();

    for (const auto & pilot_data : p_maps) {
        OPL::PilotEntry pe(pilot_data.value(OPL::Db::PILOTS_ROWID).toInt(), pilot_data);
        DB->commit(pe);
    }

    // Process Tails
    auto proc_tails = ProcessAircraft(raw_csv_data);
    proc_tails.init();
    const auto t_maps = proc_tails.getProcessedTailMaps();

    for (const auto& tail_data : t_maps) {
        OPL::TailEntry te(tail_data.value(OPL::Db::PILOTS_ROWID).toInt(), tail_data);
        DB->commit(te);
    }

    auto proc_flights = ProcessFlights(raw_csv_data,
                                       proc_pilots.getProcessedPilotsIds(),
                                       proc_tails.getProcessedTailIds());
    proc_flights.init();
    const auto flights = proc_flights.getProcessedFlights();



    for (const auto &flight_data : flights) {
        OPL::FlightEntry fe(flight_data);
        DB->commit(fe);
    }

    // Commit the exclusive transaction
    q.prepare(QStringLiteral("COMMIT"));
    q.exec();

    // destroy blocker
    blocker.unblock();
    emit DB->dataBaseUpdated(OPL::DbTable::Any);
}
}// namespace ImportCrewLongue
