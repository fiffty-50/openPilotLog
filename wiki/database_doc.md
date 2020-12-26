# Contents

- [General database layout](#general)
- [Description of the tables](#tables)
    - [flights](#flights)
    - [pilots](#pilots)
    - [tails](#tails)
    - [airports](#airports)
    - [aircraft](#aircraft)
- [Description of the views](#views)

# General database layout <a name="general"></a>

The database is a sqlite database, holding logbook data like pilots, tails and flights, as well as template data like airports or aircraft.

Apart from the tables themselves, the database also provides a number of [views](https://sqlite.org/lang_createview.html) that are used to feed [QSqlTableModel](https://doc.qt.io/qt-5/qsqltablemodel.html) and populate the [QTableViews](https://doc.qt.io/qt-5/qtableview.html) in the UI.

### Data Tables

The data tables hold data provided by the user and serve the primary functions of the program. These tables are flights, pilots and tails.

### Template Tables

The template tables hold data that is used to facilitate functionality and provide additional information that is not provided by the user. These tables are airports and aircraft. In general, the user is not expected to interface with these tables directly, but some sort of editing mechanism may be provided at a later date.

# Tables <a name="tables"></a>

## flights <a name="flights"></a>
<details><summary>Create Statement</summary>
<p>

```sql
CREATE TABLE "flights" (
    "flight_id"     INTEGER NOT NULL,
    "doft"          NUMERIC NOT NULL,
    "dept"          TEXT NOT NULL,
    "dest"          TEXT NOT NULL,
    "tofb"          INTEGER NOT NULL,
    "tonb"          INTEGER NOT NULL,
    "pic"           INTEGER NOT NULL,
    "acft"          INTEGER NOT NULL,
    "tblk"          INTEGER NOT NULL,
    "tSPSE"         INTEGER,
    "tSPME"         INTEGER,
    "tMP"           INTEGER,
    "tNIGHT"        INTEGER,
    "tIFR"          INTEGER,
    "tPIC"          INTEGER,
    "tPICUS"        INTEGER,
    "tSIC"          INTEGER,
    "tDUAL"         INTEGER,
    "tFI"           INTEGER,
    "tSIM"          INTEGER,
    "pilotFlying"   INTEGER,
    "toDay"         INTEGER,
    "toNight"       INTEGER,
    "ldgDay"        INTEGER,
    "ldgNight"      INTEGER,
    "autoland"      INTEGER,
    "secondPilot"   INTEGER,
    "thirdPilot"    INTEGER,
    "ApproachType"  TEXT,
    "FlightNumber"  TEXT,
    "Remarks"       TEXT,
    PRIMARY KEY("flight_id" AUTOINCREMENT),
    FOREIGN KEY("acft") REFERENCES "tails"("tail_id")   ON DELETE RESTRICT,
    FOREIGN KEY("pic")  REFERENCES "pilots"("pilot_id") ON DELETE RESTRICT
);
```

</p>
</details>

The flight table holds all the data relevant to a flight. Every row in the table represents a single flight. There are 9 mandatory entries that have to be `NOT NULL` when submitting a new entry. These are the basic data points required to estblish a full flight. The rest of the entries are optional and can be either determined based off the basic data, or can be chosen to be entered or omitted by the user.
### Mandatory Fields

| field   |      value      |  description |
|---------|-----------------|--------------|
| `flight_id` | INTEGER | The primary key of the table. Auto-increments when submitting, i.e. need not be provided |
| `doft` | NUMERIC | Date of flight. Format is [ISO 8601](http://www.iso.org/iso/catalogue_detail?csnumber=40874), ref. [Qt::ISODate](https://doc.qt.io/qt-5/qt.html#DateFormat-enum) |
| `dept` | TEXT | Departure Aerodrome. Format is [ICAO code](https://en.wikipedia.org/wiki/ICAO_airport_code) |
| `dest` | TEXT | Destination Aerodrome. Format is [ICAO code](https://en.wikipedia.org/wiki/ICAO_airport_code) |
| `tofb` | INTEGER | Time Off Blocks, in [UTC time](https://en.wikipedia.org/wiki/Coordinated_Universal_Time), expressed as minutes elapsed since midnight |
| `tonb` | INTEGER | Time On Blocks, in [UTC time](https://en.wikipedia.org/wiki/Coordinated_Universal_Time), expressed as minutes elapsed since midnight |
| `pic` | INTEGER | The Pilot In Command for the flight. This is a [foreign key](https://sqlite.org/foreignkeys.html) referencing pilots.pilot_id |
| `acft` | INTEGER | The aircraft flown. This is a [foreign key](https://sqlite.org/foreignkeys.html) referencing tails.tail_id |
| `tblk` | INTEGER | Total Block Time, expressed as minutes elapsed since midnight |

<details><summary>Optional fields</summary>
<p>

| field   |      value      |  description |
|---------|-----------------|--------------|
| `tSPSE` | INTEGER | Time (minutes) single pilot single engine |
| `tSPME` | INTEGER | Time (minutes) single pilot multi engine |
| `tMP` | INTEGER | Time (minutes) single pilot single engine |
| `tNIGHT` | INTEGER | Time (minutes) Night flying time |
| `tIFR` | INTEGER | Time (minutes) flown under Instrument Flight Rules |
| `tPIC` | INTEGER | Time (minutes) flown as Pilot-in-Command |
| `tPICUS` | INTEGER | Time (minutes) flown as Pilot-in-Command under supervision |
| `tSIC` | INTEGER | Time (minutes) flown as Second-in-Command (Co-Pilot) |
| `tDUAL` | INTEGER | Time (minutes) flown as training received |
| `tFI` | INTEGER | Time (minutes) flown as Flight Instructor |
| `tSIM` | INTEGER | Time (minutes) of Simulator Training (currently not implemented) |
| `pilotFlying` | INTEGER | Whether the user acted as Pilot Flying (`1`) or Pilot Monitoring (`0`) |
| `toDay` | INTEGER | Number of take-offs during day time |
| `toNight` | INTEGER | Number of take-offs during night time |
| `ldgDay` | INTEGER | Number of landings during day time |
| `ldgNight` | INTEGER | Number of landings during night time |
| `autoland` | INTEGER | Number of autolands |
| `secondPilot` | INTEGER | The second pilot of the flight. This is a [foreign key](https://sqlite.org/foreignkeys.html) referencing pilots.pilot_id |
| `thirdPilot` | INTEGER | The third pilot of the flight. This is a [foreign key](https://sqlite.org/foreignkeys.html) referencing pilots.pilot_id |
| `ApproachType` | INTEGER | The type of Approach flown |
| `FlightNumber` | INTEGER | The flight number. The airline prefix can be set in the settings dialog |
| `Remarks` | INTEGER | Remarks and endorsements |

</p>
</details>

## pilots <a name="pilots"></a>
<details><summary>Create Statement</summary>
<p>

```sql
CREATE TABLE "pilots" (
    "pilot_id"       INTEGER NOT NULL,
    "piclastname"    TEXT    NOT NULL,
    "picfirstname"   TEXT,
    "alias"          TEXT,
    "company"        TEXT,
    "employeeid"     TEXT,
    "phone"          TEXT,
    "email"          TEXT,
    "displayname"    TEXT,
    PRIMARY KEY("pilot_id" AUTOINCREMENT)
    );
```

</p>
</details>

### Mandatory Fields

| field   |      value      |  description |
|---------|-----------------|--------------|
| `pilot_id` | INTEGER | The primary key of the table, referenced by `flights.pic`. Auto-increments.
| `piclastname` | TEXT | The last name(s) of the pilot

<details><summary>Optional Fields</summary>
<p>

| field   |      value      |  description |
|---------|-----------------|--------------|
| `picfirstname` | TEXT | The first name(s) of the pilot
| `alias` | TEXT | An alias, nick-name, call-sign, etc.
| `company` | TEXT | Company
| `employeeid` | TEXT | Employee ID or crew code
| `phone` | TEXT | Phone number
| `email` | TEXT | eMail-Adress

</p>
</details>
<details><summary>Example</summary>
<p>

![example](https://raw.githubusercontent.com/fiffty-50/openpilotlog/develop/screenshots/pilots_table.png)

</p>
</details>

The pilots table holds entries of the pilots the logbook user has logged flights with. It is associated with the flights table via its primary key. 

`FOREIGN KEY("pic") REFERENCES "pilots"("pilot_id") ON DELETE RESTRICT`

In order to log a flight, at least the pilot-in-command has to be logged. A pilot entry cannot be deleted from the database if one or more flights exist where this pilot has been the PIC, since this would leave the flights table with an incomplete entry.

For a pilot entry to be valid, only the last name needs to be provided, but it is encouraged to also provide at least one first name to more easily distinguish between pilots sharing a last name.

The first entry in the pilot logbook is the logbook owner. This ensures he is easily and uniquely identified by having the pilot_id `1`. The alias field for the first entry in this database is set to `self` and is not user-editable.

## tails <a name="tails"></a>

<details><summary>Create Statement</summary>
<p>

```sql
CREATE TABLE tails ("
            " tail_id        INTEGER NOT NULL,"
            " registration   TEXT NOT NULL,"
            " company        TEXT,"
            " make           TEXT,"
            " model          TEXT,"
            " variant        TEXT,"
            " multipilot     INTEGER,"
            " multiengine    INTEGER,"
            " engineType     INTEGER,"
            " weightClass    INTEGER,"
            " PRIMARY KEY(tail_id AUTOINCREMENT)"
            ")";
```

</p>
</details>

### Mandatory Fields

| field   |      value      |  description | example |
|---------|-----------------|--------------|---------|
| `tail_id` | INTEGER | The primary key of the table, referenced by `flights.acft`. Auto-increments. | `23`
| `registration` | TEXT | The [registration](https://en.wikipedia.org/wiki/Aircraft_registration) of the aircraft | `D-LMAO`

<details><summary>Optional Fields</summary>
<p>


| field   |      value      |  description | example |
|---------|-----------------|--------------|---------|
| `company` | TEXT | The company the aircraft belongs to | `PANAM`
| `make` | TEXT | The [manufacturer](https://en.wikipedia.org/wiki/List_of_aircraft_manufacturers_by_ICAO_name) of the aircraft | `Boeing`
| `model` | TEXT | The model of the aircraft | `747`
| `variant` | TEXT | The variant of the aircroft | `400`
| `multipilot` | INTEGER | Whether the aircraft is certified for multi-pilot operations. | `1`
| `multiengine` | INTEGER | Whether the aircraft is multi-engine | `1`
| `engineType` | INTEGER | Engine Type, see below | `3`
| `weightClass` | INTEGER | [Weight Category](https://www.skybrary.aero/index.php/ICAO_Wake_Turbulence_Category), see below | `2`

Sqlite does not have a boolean data type, but 'multiengine' and 'multipilot' could be seen as such:

```
multiengine = 0 == singleengine = 1
multipilot  = 0 == singlepilot = 1
```

engineType is categorized as:
```
0 = unpowered
1 = piston Engine
2 = Turboprop
3 = Jet
```
weightClass is categorized as:
```
0 = light
1 = medium
2 = heavy
3 = super
```

</p>
</details>

### Description

The tails table holds the aircraft the user has flown and is associated with the flights table via its primary key.

`FOREIGN KEY("acft") REFERENCES "tails"("tail_id") ON DELETE RESTRICT`

In order to log a flight, the aircraft entry is mandatory. A tail entry cannot be deleted from the database if one or more flights exist with this aircraft, since this would leave the flights table with an incomplete entry.

It is important to distinguish between a tail and an aircraft.

A <b>tail</b> is unique, identified internally by its guaranteed unique `tail_id` and visible to the user with its registration (which could theoretically be non-unique but only in very rare edge cases). 

An <b>aircraft</b> is a template that holds information that tail 'instances' can share.
|table|Description
|-----|-----------------|
|`tails`|Individual aircraft|
|`aircraft`|Template for aircraft type|

> The <b>tails</b> 'D-ABAD' and 'LN-NEN' are two different aircraft, however they are both the same type of <b>aircraft</b> 'Boeing 737-800'.

## airports <a name="airports"></a>

<details><summary>Create Statement</summary>
<p>

```sql
CREATE TABLE airports
            airport_id     INTEGER NOT NULL,
            icao           TEXT NOT NULL,
            iata           TEXT,
            name           TEXT,
            lat            REAL,
            long           REAL,
            country        TEXT,
            alt            INTEGER,
            utcoffset      INTEGER,
            tzolson        TEXT,
            PRIMARY KEY(airport_id AUTOINCREMENT)
            );
```

</p>
</details>

The `airports` table holds information about airports. These include, among others, ICAO and IATA codes, common name, coordinates of their location and timezone data.

The user is in general not expected to interface with the airports table, but some limited access may be implemented at a later date, for example to change airport names or codes as they change over time.

## aircraft <a name="aircraft"></a>

<details><summary>Create Statement</summary>
<p>

```sql
CREATE TABLE aircraft (
            aircraft_id   INTEGER NOT NULL,
            make          TEXT,
            model         TEXT,
            variant       TEXT,
            name          TEXT,
            iata          TEXT,
            icao          TEXT,
            multipilot    INTEGER,
            multiengine   INTEGER,
            engineType    INTEGER,
            weightClass   INTEGER,
            PRIMARY KEY(aircraft_id AUTOINCREMENT)
            );
```

</p>
</details>

The `aircraft` table holds information about different aircraft types. This includes data like make, model, variant, engine number and type and operation. This data is used to facilitate creation of an entry in the tail database and shares most of its fields with the `tails` table.

The user is in general not expected to interface with the aircraft table, but some limited access may be implemented at a later date, for example to add new templates.

# Views <a name="views"></a>

<b> Work in Progress </b>

## defaultView

The default logbook view that shows the user a high-level overview without cluttering the screen with too much secondary information.

<details><summary>Create Statement</summary>
<p>

```sql
CREATE VIEW viewDefault AS 
SELECT 
flight_id, 
doft as 'Date', 
dept AS 'Dept', 
printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time', 
dest AS 'Dest', 
printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time ', 
printf('%02d',(tblk/60))||':'||printf('%02d',(tblk%60)) AS 'Total', 
CASE 
    WHEN pilot_id = 1 THEN alias 
    ELSE piclastname||', '||substr(picfirstname, 1, 1)||'.' 
END 
AS 'Name PIC', 
make||' '||model||'-'||variant AS 'Type', 
registration AS 'Registration', 
FlightNumber AS 'Flight #', 
Remarks 
FROM flights 
INNER JOIN pilots on flights.pic = pilots.pilot_id 
INNER JOIN tails on flights.acft = tails.tail_id 
ORDER BY date DESC
```

</p>
</details>

<details><summary>Example (light theme)</summary>
<p>

![example](https://raw.githubusercontent.com/fiffty-50/openpilotlog/develop/screenshots/default_view_system_theme.png)

</p>
</details>

## easaView

A more detailed logbook view, designed with the [EASA requirements (page 34 ff.)](https://www.easa.europa.eu/sites/default/files/dfu/Part-FCL.pdf) for logbooks in mind.

<details><summary>Create Statement</summary>
<p>

```sql
CREATE VIEW viewEASA AS 
SELECT 
flight_id, 
doft as 'Date', 
dept AS 'Dept', 
printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time', 
dest AS 'Dest', 
printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time ', 
make||' '||model||'-'||variant AS 'Type', 
registration AS 'Registration', 
(SELECT printf('%02d',(tSPSE/60))||':'||printf('%02d',(tSPSE%60)) WHERE tSPSE IS NOT "") AS 'SP SE', 
(SELECT printf('%02d',(tSPME/60))||':'||printf('%02d',(tSPME%60)) WHERE tSPME IS NOT "") AS 'SP ME', 
(SELECT printf('%02d',(tMP/60))||':'||printf('%02d',(tMP%60)) WHERE tMP IS NOT "") AS 'MP', 
printf('%02d',(tblk/60))||':'||printf('%02d',(tblk%60)) AS 'Total', 
CASE 
    WHEN pilot_id = 1 THEN alias 
    ELSE piclastname||', '||substr(picfirstname, 1, 1)||'.' 
END 
AS 'Name PIC', 
ldgDay AS 'L/D', 
ldgNight AS 'L/N', 
(SELECT printf('%02d',(tNight/60))||':'||printf('%02d',(tNight%60)) WHERE tNight IS NOT "")  AS 'Night', 
(SELECT printf('%02d',(tIFR/60))||':'||printf('%02d',(tIFR%60)) WHERE tIFR IS NOT "")  AS 'IFR', 
(SELECT printf('%02d',(tPIC/60))||':'||printf('%02d',(tPIC%60)) WHERE tPIC IS NOT "")  AS 'PIC', 
(SELECT printf('%02d',(tSIC/60))||':'||printf('%02d',(tSIC%60)) WHERE tSIC IS NOT "")  AS 'SIC', 
(SELECT printf('%02d',(tDual/60))||':'||printf('%02d',(tDual%60)) WHERE tDual IS NOT "")  AS 'Dual', 
(SELECT printf('%02d',(tFI/60))||':'||printf('%02d',(tFI%60)) WHERE tFI IS NOT "")  AS 'FI', 
Remarks 
FROM flights 
INNER JOIN pilots on flights.pic = pilots.pilot_id 
INNER JOIN tails on flights.acft = tails.tail_id 
ORDER BY date DESC
```

</p>
</details>

<details><summary>Example (dark theme)</summary>
<p>

![example](https://raw.githubusercontent.com/fiffty-50/openpilotlog/develop/screenshots/easaView_dark_theme.png)

</p>
</details>

## viewPilots

Used in the pilotsWidget. Displays information from the `pilots` table.

## viewTails

Used in the aircraftWidget. Displays information from the `tails` table.
