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
#ifndef FLIGHTSEGMENTBUILDER_H
#define FLIGHTSEGMENTBUILDER_H

#include "src/calc/nighttime.h"
#include "src/database/flightsegmententry.h"

namespace OPL {

/**
 * \brief Utility for constructing contiguous flight time segments.
 *
 * Builds time segments from minute-based event data. Each segment represents
 * a continuous interval where all flight-related properties (e.g. light
 * conditions, flight rules, pilot function) remain constant.
 *
 * The following rules apply
 * - start_ms is inclusive
 * - end_ms is exclusive
 * - duration = end_ms - start_ms
 *
 * Segments fully cover the input time range without gaps or overlap.
 */
class FlightSegmentBuilder {

  public:
    FlightSegmentBuilder() = default;

    /*!
     * \brief Encapsulates all data needed to create a FlightSegmentEntry
     */
    struct SegmentData {
        int start_ms;
        int end_ms;
        FlightSegmentEntry::Optionals opts;
    };

    /*!
     * \brief Build segments from night/day event data.
     *
     * Converts a sequence of minute-resolution night time events into a list
     * of contiguous time segments. A new segment is created whenever the
     * night/day state changes.
     *
     * \param opts   Base options applied to all segments.
     */
    static QList<SegmentData> fromNightTime(const QList<NightTime::Event> &events,
                                            const FlightSegmentEntry::Optionals &opts = {});

  private:
    static constexpr int MS_PER_MIN = 60'000;
};

} // namespace OPL
#endif // FLIGHTSEGMENTBUILDER_H
