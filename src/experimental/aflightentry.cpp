#include "aflightentry.h"
#include "src/experimental/adatabase.h"

namespace experimental {

AFlightEntry::AFlightEntry()
    : AEntry::AEntry(DEFAULT_FLIGHT_POSITION)
{}

AFlightEntry::AFlightEntry(int row_id)
    : AEntry::AEntry(DataPosition("flights", row_id))
{}

AFlightEntry::AFlightEntry(TableData table_data)
    : AEntry::AEntry(DEFAULT_FLIGHT_POSITION, table_data)
{}

QString AFlightEntry::summary()
{
    if(tableData.isEmpty())
        return QString();

    QString flight_summary;
    flight_summary.append(tableData.value("doft") + " ");
    flight_summary.append(tableData.value("dept") + " ");
    flight_summary.append(ACalc::minutesToString(tableData.value("tofb")) + " ");
    flight_summary.append(ACalc::minutesToString(tableData.value("tonb")) + " ");
    flight_summary.append(tableData.value("dest") + " ");

    return flight_summary;
}

QString AFlightEntry::registration()
{
    QString tail_id = tableData.value("acft");
    if(tail_id.isEmpty())
        return QString();

    QString statement = "SELECT registration "
                        "FROM tails "
                        "WHERE ROWID =" + tail_id;

    auto tail_registration = aDB()->customQuery(statement, 1);

    if(tail_registration.isEmpty()) {
        return QString();
    } else {
        return tail_registration.first();
    }
}

QString AFlightEntry::pilotName(pilot pilot_)
{
    QString row_id;
    switch (pilot_) {
    case pilot::pic:
        row_id = tableData.value("pic");
        break;
    case pilot::sic:
        row_id = tableData.value("sic");
        break;
    case pilot::thirdPilot:
        row_id = tableData.value("thirdPilot");
        break;
    }
    if(row_id == QString())
        return row_id;

    QString statement = "SELECT piclastname||\", \"||picfirstname "
                        "FROM pilots "
                        "WHERE ROWID =" + row_id;

    auto pilot_name = aDB()->customQuery(statement, 1);
    if(pilot_name.isEmpty()) {
        return QString();
    } else {
        return pilot_name.first();
    }
}

} // namespace experimental
