# Aircraft

This table holds aircraft types. An aircraft type holds data that all aircraft of this type share. Instances of specific aircraft are stored in the tails table. This table can hold the information for an Airbus A320 or Boeing 737, but does not contain a specific aircraft of the type.

## Fields

| Column | Type | Description |
| --- | --- | --- |
| 'aircraft_id' | INTEGER | Primary Key |
| 'make' | TEXT NOT NULL | Manufacturer e.g. Boeing|
| 'model' | TEXT NOT NULL | Model e.g. 737 |
| 'variant' | TEXT | Variant, e.g. 800 |
| 'icao_designator' | TEXT | According ICAO Doc 8643 |
| 'engine_type' | TEXT | Check in ('UNPOWERED', 'PISTON', 'TURBOPROP', 'JET', 'ELECTRIC', 'OTHER') |
| 'engine_count' | INTEGER | Anything > 1 means Multi-Engine |
| 'class' | TEXT | Check in ('AEROPLANE', 'HELICOPTER', 'GLIDER', 'ROTORCRAFT', 'DRONE', 'OTHER') |
| 'sub_class' | TEXT | Check in ('LAND', 'SEA', 'AMPHIBIAN') |
| 'is_multi_pilot' | INTEGER | Check in (0,1) |
| 'wake_category' | TEXT | Check in ('LIGHT', 'MEDIUM', 'HEAVY', 'SUPER')
| 'type_rating' | TEXT | optional |

## Notes

`is_multi_pilot` is stored here since most aircraft are either single or multi-pilot. Some aircraft (business jets) can however be operated either way. So the value stored here should not be used for querying multi-pilot time. It is rather used to set a default value for single- or multi-pilot operation in the flight_segments table.