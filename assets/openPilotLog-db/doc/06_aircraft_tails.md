
# Tails

This table holds specific aircrafts. The minimum required information is a type from the aircrafts table and a registration.

A registration must be unique at a given point in time. This means that sometimes registrations are re-used for different tails, but the same registration is only ever given to a new tail when the old one has been retired. To cater for this, 'in_service' and 'out_of_service' can be set.

## Fields
| Column | Type | Description |
| --- | --- | --- |
| 'tail_id' | INTEGER | Primary Key |
| 'aircraft_id' | INTEGER | Foreign Key to aircrafts table |
| 'registration' | TEXT NOT NULL | Must be unique |
| 'in_service_jd' | INTEGER | Start date julian day |
| 'out_of_service_jd' | INTEGER | End date julian day |
| 'company' | TEXT | optional |
| 'remarks' | TEXT | optional |

## Indexes

### Search aircraft by registration
CREATE INDEX idx_tails_registration ON tails(registration);

### Find all aircraft of a type
CREATE INDEX idx_tails_aircraft_type ON tails(aircraft_type_id);