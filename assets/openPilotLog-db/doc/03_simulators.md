# Simulators

The simulators table holds simulator sessions not performed on real aircraft.

Simulator sessions share many of the same attributes but have a couple of important distinctions
- While they can have a registration, this is not mandatory as for aircraft
- There are different types of simulators (FNPT/FFS) and some types can create regulatory credit while others can not
- Simulator time does not count towards flight time but may count towards minimum time requirements for the purpose of obtaining a qualification
- Take offs and Landings performed in specific simulators (FFS) can count towards regulatory requirements for type rating currency

## Fields

| Column | Type | Description |
| --- | --- | --- |
| 'sim_id' | INTEGER | Primary Key |
| 'event_id' | INTEGER | Foreign Key to log_events |
| 'sim_type' | TEXT | FNPT I / FNPT II / FFS |
| 'aircraft_type_id' | INTEGER | Foreign Key to aircraft (optional) |
| 'duration_ms' | INTEGER  | The duration of the session (ms)|
| 'instructor_id' | INTEGER | Foreign Key to pilots table |

## Notes

If a simulator session can create regulatory credit for take-off and landing currency, this will be handled in a seperate table (not yet implemented).

