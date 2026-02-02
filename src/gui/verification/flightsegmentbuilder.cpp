/*
 *openPilotLog - A FOSS Pilot Logbook Application
 *Copyright (C) 2020-2026 Felix Turowsky
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
#include "flightsegmentbuilder.h"

namespace OPL {

QList<FlightSegmentBuilder::SegmentData>
OPL::FlightSegmentBuilder::fromNightTime(const QList<NightTime::Event> &events,
                                         const FlightSegmentEntry::Optionals &opts)
{
    if (events.size() < 2) return {};

    QList<SegmentData> ret;
    ret.reserve(events.size());

    // get the first event
    const auto &start_event = events.first();
    bool is_night           = start_event.isNight;
    int start_ms            = start_event.timeMs;
    int end_ms              = start_ms;

    // loop through the events starting at the second element
    for (auto it = std::next(events.cbegin()); it != events.cend(); ++it) {
        // resolution of night time is one minute
        end_ms += MS_PER_MIN;

        // on a change of conditions, end the current segment and save it
        if (it->isNight != is_night) {
            auto optionals     = opts;
            optionals.is_night = is_night;
            ret.append({start_ms, end_ms, optionals});

            // start new segment exactly at previous end
            start_ms = end_ms;
            is_night = it->isNight;
        }
    }

    // close the final segment
    auto optionals     = opts;
    optionals.is_night = is_night;
    ret.append({start_ms, end_ms, optionals});

    return ret;
}

} // namespace OPL
