
# Airports

This table holds airports. An airport is identified by its unique id and represents the physical location of an airport, as this is the most stable attribute. Both ICAO as well as IATA codes are sometimes changed (event though ICAO codes are more stable).

For ICAO and IATA codes, 'from' and 'to' dates can be specified for when such a change occurs and they are stored in a seperate table.

| Column | Type | Description |
| --- | --- | --- |
| 'airport_id' | INTEGER | Primary Key |
| 'airport_name' | TEXT NOT NULL | The text name of the airport |
| 'latitude' | REAL NOT NULL | latitude according WGS84 |
| 'longitude' | REAL NOT NULL | longitude according WGS84 |
| 'timezone_olson' | TEXT | optional |
