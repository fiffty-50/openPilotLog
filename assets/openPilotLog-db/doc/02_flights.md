# Flights

The flights table hold flights, which have to be on real aircraft. It is linked to log_events and holds only basic flight data. Associated data such as segmnent time, function time or derived values light night time are stored in a seperate table.

The minimum required values for a flight are
- Date (stored in log_events)
- Departure Aerodrome
- Time Off Blocks
- Destination Aerodrome
- Time On Blocks
- Pilot in Command
- Tail (a specific aircraft)

## Fields
| Column | Type | Description |
| --- | --- | --- |
| 'flight_id' | INTEGER | Primary Key |
| 'event_id' | INTEGER NOT NULL | Foreign Key to log_events |
| 'departure' | INTEGER NOT NULL | Foreign Key to airports table ON DELETE RESTRICT |
| 'destination' | INTEGER NOT NULL | Foreign Key to airports table ON DELETE RESTRICT |
| 'tofb_ms' | INTEGER NOT NULL | Time Off Blocks (ms) |
| 'tonb_ms' | INTEGER NOT NULL | Time On Blocks (ms)|
| 'pic_pilot_id' | INTEGER NOT NULL | Foreign Key to Pilots table, ON DELETE RESTRICT |
| 'second_pilot_id' | INTEGER | Foreign Key to Pilots table, ON DELETE RESTRICT |
| 'third_pilot_id' | INTEGER | Foreign Key to Pilots table, ON DELETE RESTRICT |
| 'fourth_pilot_id' | INTEGER | Foreign Key to Pilots table, ON DELETE RESTRICT |
| 'tail' | INTEGER NOT NULL | Foreign Key to Tails table, ON DELETE RESTRICT |
| 'flight_number' | TEXT | optional |

## Notes

Similar to how date values are stored as julian days, time values are stored as milliseconds elapsed since midnight.

Block Times may cross midnight, but will never cross midnight more than once. This is true for the great majority of all flights and catering to the very extreme exceptions to this is not worth the effort. The longest commercial flights are around 18 hours in length.

Block Time and other times are not stored here, but derived and stored in flight_segments.
Take off and landings as well as approach types are stored in their respective tables.

## Indexes

### Link back to log_events
CREATE INDEX idx_flights_event_id ON flights(event_id);

### Find flights by aircraft
CREATE INDEX idx_flights_tail ON flights(tail);

### Find flights by departure/destination (route analysis)
CREATE INDEX idx_flights_departure ON flights(departure);
CREATE INDEX idx_flights_destination ON flights(destination);

### PIC 
CREATE INDEX idx_flights_pic ON flights(pic);

