# Log Events

This is the top level table for all loggable events in the database. 

A loggable event can be a flight (on a real aircraft) or a simulator session. Since simulator time does not count as flight time, these entries are kept seperate.

Under some conditions, simulator events can create credit for regulatory requirements, for example
- Simulator Training Time as part of the minimum required time for a licence
- Take off and landing currency requirements can be met by performing those in suitable simulators

## Fields

| Column | Type | Description
| --- | --- | --- |
| 'event_id' | INTEGER | Primary Key |
| 'event_type' | TEXT NOT NULL | CHECK entry_type in ('FLIGHT', 'SIM') |
| 'event_date_jd' |INTEGER NOT NULL | Date is stored as a julian day |
| 'remarks' | TEXT | optional remarks |

## Notes

Every event has to have a date specified. In order to speed up access and calculations this date is stored as a [Julian Day](https://en.wikipedia.org/wiki/Julian_day). 

## Indexes

### Search flights by pilot
CREATE INDEX idx_log_events_pilot_id ON log_events(pilot_id);

### Search flights by tail
CREATE INDEX idx_log_events_tail_id ON log_events(tail_id);

### Search flights by airport
CREATE INDEX idx_log_events_airport_id ON log_events(airport_id);

### Filter by date range
CREATE INDEX idx_log_events_event_date ON log_events(event_date_jd);

### Find simulator sessions vs real flights
CREATE INDEX idx_log_events_event_type ON log_events(event_type);