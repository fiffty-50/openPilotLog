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
#ifndef TIME_H
#define TIME_H

#include <QtCore>
namespace OPL {

namespace Time {
static constexpr int MINUTES_PER_DAY = 24 * 60;
static constexpr int MSECS_PER_DAY =
    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::hours(24)).count();

/*!
 * \brief Determines whether a given amount of time in milliseconds since midnight is a valid
 * time of day.
 */
static constexpr bool isValidTimeOfDay(int milliseconds)
{
    return milliseconds >= 0 && milliseconds < MSECS_PER_DAY;
};

/*!
 * \brief A valid block time must be 0 < time < 24 hours
 */
static constexpr bool isValidBlockTime(int milliseconds)
{
    return milliseconds > 0 && milliseconds < MSECS_PER_DAY;
}

/*!
 * \brief Calculate the block time in milliseconds from start to end
 * \return The block time or -1 for an invalid result
 */
static constexpr int blockTimeMs(int start_ms, int end_ms)
{
    if (!isValidTimeOfDay(start_ms) || !isValidTimeOfDay(end_ms)) return -1;

    return (end_ms - start_ms + MSECS_PER_DAY) % MSECS_PER_DAY;
}

/*!
 * \brief Calculate the block time in milliseconds from start to end
 * \return The block time or -1 for an invalid result
 */
static const inline int blockTimeMs(const QTime &start, const QTime &end)
{
    if (!start.isValid() || !end.isValid()) return -1;

    int ms = start.msecsTo(end);
    if (ms < 0) ms += MSECS_PER_DAY;

    return ms;
}

} // namespace Time
} // namespace OPL

#endif // TIME_H
