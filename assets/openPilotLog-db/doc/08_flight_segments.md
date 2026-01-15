
# Flight Segments

The flight segment table holds continuous time intervals within a flight during which conditions are homogenous.

Every flight has one or more segments and the union of the segments equals the flights block time.

On a `normal` flight, only one segment exists for the whole flight. There are however edge cases where during a flight certain conditions change that need to be catered for. An example would be flight that starts as a VFR flight, but becomes an IFR flight later on and vice versa. This segmentation also enables transfers of pilot functions as well as logging of day vs night time.

All time values are stored as ms elapsed since start of day.

## Fields

| Column | Type | Description |
| --- | --- | --- |
| 'segment_id' | INTEGER | Primary Key |
| 'flight_id' | INTEGER | Foreign key to flights table |
| 'start_ms' | INTEGER NOT NULL | Start time of segment |
| 'end_ms' | INTEGER NOT NULL | end time of segment |
| 'is_ifr' | INTEGER | Check in (0,1) where 0 == VFR, 1 == IFR |
| 'is_simulated_ifr' | INTEGER | Check in (0,1) where 0 == actual IFR, 1 == simulated IFR |
| 'is_night' | INTEGER | Check in (0,1) where 0 == day, 1 == night |
| 'is_multi_pilot' | INTEGER | Check in (0,1) where 0 == Single Pilot, 1 == Multi Pilot |
| 'pilot_function' | TEXT | Check in ('PIC', 'PICUS', 'SIC', 'DUAL', 'FI' |

INDEX (flight_id)

## Notes

Flight segments are replacable but shall be edited as a whole. When a flight is edited, **ALL** its segments are deleted and recreated. This is preferred over mutating partial segment data.

Within one segment
- Flight rules do not change
- Light conditions do not change
- Logged pilot function does not change

For Time Segments
- 'start_ms' is **inclusive**
- 'end_ms' is **exclusive**
- -> Duration: 'end_ms - start_ms'
- Segments must cover the full block time without gaps or overlaps

Care must be taken for logging night time properly as flights can transit from day to night more than once during a flight.


