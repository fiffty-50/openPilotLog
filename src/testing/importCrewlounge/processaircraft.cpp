#include "processaircraft.h"
#include "src/database/tailentry.h"

void ProcessAircraft::parseRawData()
{
    QStringList tail_details;
    // relevant colums: {reg,company,make,model,variant,multipilot,multiengine,engineType,weightClass}
    int relevant_cols[9] = {79, 36, 76, 77, 78, 83, 84, 92, 96};

    for (const auto &row : std::as_const(rawData)) {
        for (const auto &col : relevant_cols) {
            tail_details.append(row[col]);
        }
        if (!(unique_tails.contains(tail_details)))
            unique_tails.append(tail_details);
        tail_details.clear();
    }
}

void ProcessAircraft::processParsedData()
{
    // init counter
    int unique_tail_id = 1;

    QHash<QString, QVariant> new_tail_data;
    for (const auto &list : std::as_const(unique_tails)) {
        new_tail_data.insert(OPL::TailEntry::REGISTRATION, list[0]);
        new_tail_data.insert(OPL::TailEntry::COMPANY, list[1]);
        new_tail_data.insert(OPL::TailEntry::MAKE, list[2]);
        new_tail_data.insert(OPL::TailEntry::MODEL, list[3]);
        new_tail_data.insert(OPL::TailEntry::VARIANT, list[4]);

        if (list[5] == "TRUE")
            new_tail_data.insert(OPL::TailEntry::MULTI_PILOT, 1);
        else
            new_tail_data.insert(OPL::TailEntry::MULTI_PILOT, 0);

        if (list[6] == "TRUE")
            new_tail_data.insert(OPL::TailEntry::MULTI_ENGINE, 1);
        else
            new_tail_data.insert(OPL::TailEntry::MULTI_ENGINE, 0);

        if (list[7] == "Piston") // other values need to be added as needed, do later
            new_tail_data.insert(OPL::TailEntry::ENGINE_TYPE, 1);
        else if (list[7] == "Turbine (jet-fan)")
            new_tail_data.insert(OPL::TailEntry::ENGINE_TYPE, 3);

        if (list[8] == "TRUE") // this is a above 7.5t switch in MCC, so default to medium for now
            new_tail_data.insert(OPL::TailEntry::WEIGHT_CLASS, 1);
        else
            new_tail_data.insert(OPL::TailEntry::WEIGHT_CLASS, 0);

        new_tail_data.insert(QStringLiteral("tail_id"), unique_tail_id);

        processedTailIds.insert(list[0], unique_tail_id);
        processedTailMaps.insert(list[0], new_tail_data);
        unique_tail_id ++;
        new_tail_data.clear();
    }
}

QHash<QString, int> ProcessAircraft::getProcessedTailIds() const
{
    return processedTailIds;
}

QHash<QString, QHash<QString, QVariant> > ProcessAircraft::getProcessedTailMaps() const
{
    return processedTailMaps;
}
