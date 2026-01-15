# Approach Events

Similar to movement events, but for approaches. A `normal` flight only has one approach, but during training flights or after a missed approach more than one approach can be performed during a flight.

## Fields

| Column | Type | Description |
| --- | --- | --- |
| 'approach_id' | INTEGER | Primary Key |
| 'event_id' | INTEGER | Foreign key to log_events |
| 'airport_id' | INTEGER | Foreign key to airports |
| 'approach_type' | TEXT | 'ILS','RNAV', 'VISUAL', 'CIRCLING' etc. |

## Notes
If an approach is a 'VISUAL' approach, it does not count as an instrument approach. Make sure to check for this in query design.

## Indexes

### Find approaches for a flight
CREATE INDEX idx_approach_events_flight_id ON approach_events(flight_id);
