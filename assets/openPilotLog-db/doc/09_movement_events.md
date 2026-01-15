# Movement Events

This table stores movement events and stores data similar to flight_segments. It differs in that its event are not time-based. They are tied to a log entry, as they are happening within a flight segment but don't necessarily belong to them, since they may overlap (approaches) or happen more than once during a segment (touch-and-go landings).

# Fields

| Column | Type | Description |
| --- | --- | --- |
| 'movement_id' | INTEGER | Primary Key |
| 'event_id' | INTEGER | Foreign Key to log_events |
| 'airport_id' | INTEGER | Foreign Key to airports |
| 'is_landing' | INTEGER | 0 == Take Off, 1 == Landing |
| 'is_night' | INTEGER | 0 == Day, 1 == Night |
| 'is_autoland' | INTEGER | 0 == manual, 1 == autoland |

## Notes

Touch and Go's are handled in the UI and result in a take off and a landing

## Indexes

### Find all takeoffs/landings for a flight
CREATE INDEX idx_movement_events_flight_id ON movement_events(flight_id);

### Filter by movement type (takeoffs vs landings)
CREATE INDEX idx_movement_events_type ON movement_events(movement_type);