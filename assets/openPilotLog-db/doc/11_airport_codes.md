# Airport Codes

This table stores IATA and ICAO codes for physical airports.
All codes havE a valid_from and valid_to date as julian days.

## Fields

| Column | Type | Description |
| --- | --- | --- |
| `airport_code_id` | INTEGER |  Primary Key |
| `airport_id` | INTEGER | Foreign Key to airports table |
| `code` | TEXT NOT NULL | The airport code (icao or iata) |
| `code_type` | TEXT NOT NULL | Check in ('ICAO', 'IATA', 'OTHER') |
| `valid_from_jd` | INTEGER | Start date of Code Validity |
| `valid_to_jd` | INTEGER | End date of Code Validity |


## Notes

If no end date is specified (NULL), the code is currently valid.
Commencement and expiry dates are busy work for the future, for now all airports will be set as valid from the start of the 20th century.

Validity internal must not overlap for the same airport:

`CHECK (valid_to_jd IS NULL OR valid_to_jd > valid_from_jd)`

## Indexes

```
INDEX (airport_id, code_type, valid_from_jd)
UNIQUE (code_type, code, valid_from_jd)
```

