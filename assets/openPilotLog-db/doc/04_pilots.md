# Pilots

This table holds identities for pilots. A pilot is identified by the primary key, but its name must be set. Additional data can be entered here as well.

The name of the pilot is not split into first and last name. This gives the user more flexibility.

## Fields


| Column | Type | Description |
| --- | --- | --- |
| 'pilot_id' | INTEGER | Primary Key |
| 'pilot_name' | TEXT NOT NULL | Name of the pilot |
| 'alias' | TEXT | An alias or nickname |
| 'employee_id' | TEXT | Most airlines use these |
| 'company' | TEXT | Company the pilot works for |
| 'phone' | TEXT | Phone number. Stored as text to allow for country codes |
| 'email' | TEXT | eMail address |
| 'remarks' | TEXT |optional |


## Notes

This table holds all pilots, regardless of rank or function as well as instructors or examiners.