DROP TABLE IF EXISTS 'pilots';
CREATE TABLE IF NOT EXISTS 'pilots' (
	'pilot_id'	INTEGER NOT NULL,
	'lastname'	TEXT NOT NULL,
	'firstname'	TEXT,
        'alias'		TEXT,
	'company'	TEXT,
	'employeeid'	TEXT,
        'phone'		TEXT,
        'email'		TEXT,
	PRIMARY KEY('pilot_id' AUTOINCREMENT)
);
DROP TABLE IF EXISTS 'tails';
CREATE TABLE IF NOT EXISTS 'tails' (
	'tail_id'	INTEGER NOT NULL,
	'registration'	TEXT NOT NULL,
	'company'	TEXT,
        'make'		TEXT,
        'model'		TEXT,
	'variant'	TEXT,
	'multipilot'	INTEGER,
	'multiengine'	INTEGER,
	'engineType'	INTEGER,
	'weightClass'	INTEGER,
	PRIMARY KEY('tail_id' AUTOINCREMENT)
);
DROP TABLE IF EXISTS 'flights';
CREATE TABLE IF NOT EXISTS 'flights' (
	'flight_id'	INTEGER NOT NULL,
        'doft'		NUMERIC NOT NULL,
        'dept'		TEXT NOT NULL,
        'dest'		TEXT NOT NULL,
        'tofb'		INTEGER NOT NULL,
        'tonb'		INTEGER NOT NULL,
        'pic'		INTEGER NOT NULL,
        'acft'		INTEGER NOT NULL,
        'tblk'		INTEGER NOT NULL,
        'tSPSE'		INTEGER,
        'tSPME'		INTEGER,
        'tMP'		INTEGER,
	'tNIGHT'	INTEGER,
        'tIFR'		INTEGER,
        'tPIC'		INTEGER,
	'tPICUS'	INTEGER,
        'tSIC'		INTEGER,
        'tDUAL'		INTEGER,
        'tFI'		INTEGER,
        'tSIM'		INTEGER,
	'pilotFlying'	INTEGER,
        'toDay'		INTEGER,
	'toNight'	INTEGER,
	'ldgDay'	INTEGER,
	'ldgNight'	INTEGER,
	'autoland'	INTEGER,
	'secondPilot'	INTEGER,
	'thirdPilot'	INTEGER,
	'approachType'	TEXT,
	'flightNumber'	TEXT,
	'remarks'	TEXT,
	FOREIGN KEY('pic') REFERENCES 'pilots'('pilot_id') ON DELETE RESTRICT,
	FOREIGN KEY('acft') REFERENCES 'tails'('tail_id') ON DELETE RESTRICT,
	PRIMARY KEY('flight_id' AUTOINCREMENT)
);
DROP TABLE IF EXISTS 'aircraft';
CREATE TABLE IF NOT EXISTS 'aircraft' (
	'aircraft_id'	INTEGER NOT NULL,
        'make'		TEXT,
        'model'		TEXT,
	'variant'	TEXT,
        'name'		TEXT,
        'iata'		TEXT,
        'icao'		TEXT,
	'multipilot'	INTEGER,
	'multiengine'	INTEGER,
	'engineType'	INTEGER,
	'weightClass'	INTEGER,
	PRIMARY KEY('aircraft_id' AUTOINCREMENT)
);
DROP TABLE IF EXISTS 'airports';
CREATE TABLE IF NOT EXISTS 'airports' (
	'airport_id'	INTEGER NOT NULL,
        'icao'		TEXT NOT NULL,
        'iata'		TEXT,
        'name'		TEXT,
        'lat'		REAL,
        'long'		REAL,
	'country'	TEXT,
	'tzolson'	TEXT,
	PRIMARY KEY('airport_id' AUTOINCREMENT)
);
DROP TABLE IF EXISTS 'currencies';
CREATE TABLE IF NOT EXISTS "currencies" (
        "currency_id"	INTEGER NOT NULL,
        "currencyName"	TEXT,
        "expiryDate"	NUMERIC,
        PRIMARY KEY('currency_id' AUTOINCREMENT)
);
DROP TABLE IF EXISTS 'changelog';
CREATE TABLE IF NOT EXISTS 'changelog' (
	'revision'	INTEGER NOT NULL,
	'comment'	TEXT,
        'date'		NUMERIC,
	PRIMARY KEY('revision' AUTOINCREMENT)
);
DROP TABLE IF EXISTS 'simulators';
CREATE TABLE IF NOT EXISTS 'simulators' (
	'session_id'	INTEGER NOT NULL,
        'date'		NUMERIC NOT NULL,
	'totalTime'	INTEGER NOT NULL,
	'deviceType'	TEXT NOT NULL,
	'aircraftType'	TEXT,
	'registration'	TEXT,
	'remarks'	TEXT,
	PRIMARY KEY('session_id' AUTOINCREMENT)
);
DROP TABLE IF EXISTS 'previousExperience';
CREATE TABLE 'previousExperience' (
        'tblk'		INTEGER,
        'tSPSE'		INTEGER,
        'tSPME'		INTEGER,
        'tMP'		INTEGER,
        'tNIGHT'	INTEGER,
        'tIFR'		INTEGER,
        'tPIC'		INTEGER,
        'tPICUS'	INTEGER,
        'tSIC'		INTEGER,
        'tDUAL'		INTEGER,
        'tFI'		INTEGER,
        'tSIM'		INTEGER,
        'toDay'		INTEGER,
        'toNight'	INTEGER,
        'ldgDay'	INTEGER,
        'ldgNight'	INTEGER,
        'autoland'	INTEGER
);
DROP VIEW IF EXISTS 'viewDefault';
CREATE VIEW viewDefault AS  
SELECT 	flight_id,
        doft as 'Date',
        dept AS 'Dept',
        printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time',
        dest AS 'Dest', printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time ',
        printf('%02d',(tblk/60))||':'||printf('%02d',(tblk%60)) AS 'Total',
        CASE  WHEN pilot_id = 1 THEN alias  ELSE lastname||', '||substr(firstname, 1, 1)||'.'  END  AS 'Name PIC',
        CASE  WHEN variant IS NOT NULL THEN make||' '||model||'-'||variant  ELSE make||' '||model  END  AS 'Type',
        registration AS 'Registration',
        FlightNumber AS 'Flight #',
        remarks AS 'Remarks'
FROM flights  
INNER JOIN pilots on flights.pic = pilots.pilot_id
INNER JOIN tails on flights.acft = tails.tail_id
ORDER BY date DESC;

DROP VIEW IF EXISTS 'viewDefaultSim';
CREATE VIEW viewDefaultSim AS 
SELECT 	flight_id AS 'rowid',
        doft as 'Date',
        dept AS 'Dept',
        printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time',
        dest AS 'Dest', printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time ',
        printf('%02d',(tblk/60))||':'||printf('%02d',(tblk%60)) AS 'Total',
        CASE  WHEN pilot_id = 1 THEN alias  ELSE lastname||', '||substr(firstname, 1, 1)||'.'  END  AS 'Name PIC',
        CASE  WHEN variant IS NOT NULL THEN make||' '||model||'-'||variant  ELSE make||' '||model  END  AS 'Type',
        registration AS 'Registration',
        null AS 'Sim Type',
        null AS 'Time of Session',
        remarks AS 'Remarks'
FROM flights   
INNER JOIN pilots on flights.pic = pilots.pilot_id  
INNER JOIN tails on flights.acft = tails.tail_id   
UNION 
        SELECT (session_id * -1),
        date,
        null, null, null, null,
        'SIM',
        null,
        aircraftType,
        registration,
        deviceType,
        printf('%02d',(totalTime/60))||':'||printf('%02d',(totalTime%60)),
        remarks
FROM simulators 
ORDER BY date DESC;

DROP VIEW IF EXISTS 'viewEasa';
CREATE VIEW viewEasa AS  SELECT  flight_id, 
        doft as 'Date',
        dept AS 'Dept',
        printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time',
        dest AS 'Dest',
        printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time ',
        CASE  WHEN variant IS NOT NULL THEN make||' '||model||'-'||variant  ELSE make||' '||model  END  AS 'Type',
        registration AS 'Registration',
        (SELECT printf('%02d',(tSPSE/60))||':'||printf('%02d',(tSPSE%60)) WHERE tSPSE IS NOT NULL) AS 'SP SE',
        (SELECT printf('%02d',(tSPME/60))||':'||printf('%02d',(tSPME%60)) WHERE tSPME IS NOT NULL) AS 'SP ME',
        (SELECT printf('%02d',(tMP/60))||':'||printf('%02d',(tMP%60)) WHERE tMP IS NOT NULL) AS 'MP',
        printf('%02d',(tblk/60))||':'||printf('%02d',(tblk%60)) AS 'Total',
        CASE  WHEN pilot_id = 1 THEN alias  ELSE lastname||', '||substr(firstname, 1, 1)||'.'  END  AS 'Name PIC',
        ldgDay AS 'L/D',
        ldgNight AS 'L/N',
        (SELECT printf('%02d',(tNight/60))||':'||printf('%02d',(tNight%60)) WHERE tNight IS NOT NULL)  AS 'Night',
        (SELECT printf('%02d',(tIFR/60))||':'||printf('%02d',(tIFR%60)) WHERE tIFR IS NOT NULL)  AS 'IFR',
        (SELECT printf('%02d',(tPIC/60))||':'||printf('%02d',(tPIC%60)) WHERE tPIC IS NOT NULL)  AS 'PIC',
        (SELECT printf('%02d',(tSIC/60))||':'||printf('%02d',(tSIC%60)) WHERE tSIC IS NOT NULL)  AS 'SIC',
        (SELECT printf('%02d',(tDual/60))||':'||printf('%02d',(tDual%60)) WHERE tDual IS NOT NULL)  AS 'Dual',
        (SELECT printf('%02d',(tFI/60))||':'||printf('%02d',(tFI%60)) WHERE tFI IS NOT NULL)  AS 'FI',
        remarks AS 'Remarks'
FROM flights  
INNER JOIN pilots on flights.pic = pilots.pilot_id  
INNER JOIN tails on flights.acft = tails.tail_id  ORDER BY date DESC;

DROP VIEW IF EXISTS 'viewEasaSim';
CREATE VIEW viewEasaSim AS  SELECT  flight_id, 
        doft as 'Date',
        dept AS 'Dept',
        printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time',
        dest AS 'Dest',
        printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time ',
        CASE  WHEN variant IS NOT NULL THEN make||' '||model||'-'||variant  ELSE make||' '||model  END  AS 'Type',
        registration AS 'Registration',
        (SELECT printf('%02d',(tSPSE/60))||':'||printf('%02d',(tSPSE%60)) WHERE tSPSE IS NOT NULL) AS 'SP SE',
        (SELECT printf('%02d',(tSPME/60))||':'||printf('%02d',(tSPME%60)) WHERE tSPME IS NOT NULL) AS 'SP ME',
        (SELECT printf('%02d',(tMP/60))||':'||printf('%02d',(tMP%60)) WHERE tMP IS NOT NULL) AS 'MP',
        printf('%02d',(tblk/60))||':'||printf('%02d',(tblk%60)) AS 'Total',
        CASE  WHEN pilot_id = 1 THEN alias  ELSE lastname||', '||substr(firstname, 1, 1)||'.'  END  AS 'Name PIC',
        ldgDay AS 'L/D',
        ldgNight AS 'L/N',
        (SELECT printf('%02d',(tNight/60))||':'||printf('%02d',(tNight%60)) WHERE tNight IS NOT NULL)  AS 'Night',
        (SELECT printf('%02d',(tIFR/60))||':'||printf('%02d',(tIFR%60)) WHERE tIFR IS NOT NULL)  AS 'IFR',
        (SELECT printf('%02d',(tPIC/60))||':'||printf('%02d',(tPIC%60)) WHERE tPIC IS NOT NULL)  AS 'PIC',
        (SELECT printf('%02d',(tSIC/60))||':'||printf('%02d',(tSIC%60)) WHERE tSIC IS NOT NULL)  AS 'SIC',
        (SELECT printf('%02d',(tDual/60))||':'||printf('%02d',(tDual%60)) WHERE tDual IS NOT NULL)  AS 'Dual',
        (SELECT printf('%02d',(tFI/60))||':'||printf('%02d',(tFI%60)) WHERE tFI IS NOT NULL)  AS 'FI',
        null AS 'Sim Type',
        null AS 'Time of Session',
        remarks AS 'Remarks'
FROM flights    
INNER JOIN pilots on flights.pic = pilots.pilot_id    
INNER JOIN tails on flights.acft = tails.tail_id    
UNION  
SELECT (session_id * -1),  
        date,
        null,  null,  null,  null,
        aircraftType,
        registration,
        null,  null,  null,
        'SIM',
        null,  null,  null,  null,  null,  null,  null,  null,  null,
        deviceType,  printf('%02d',(totalTime/60))||':'||printf('%02d',(totalTime%60)),
        remarks
FROM simulators  
ORDER BY date DESC;

DROP VIEW IF EXISTS 'viewSimulators';
CREATE VIEW viewSimulators AS SELECT (session_id * -1),  
        date as 'Date',
        registration AS 'Registration',
        aircraftType AS 'Aircraft Type',
        deviceType 'Sim Type',
        printf('%02d',(totalTime/60))||':'||printf('%02d',(totalTime%60)) AS 'Time of Session',
        remarks AS 'Remarks'
FROM simulators  
ORDER BY date DESC;

DROP VIEW IF EXISTS 'viewTails';
CREATE VIEW viewTails AS  
SELECT  tail_id AS 'ID',  
        registration AS 'Registration',
        make||' '||model AS 'Type',
        company AS 'Company'
FROM tails WHERE model IS NOT NULL AND variant IS NULL  
UNION  
SELECT  tail_id AS 'ID',  
        registration AS 'Registration',
        make||' '||model||'-'||variant AS 'Type',
        company AS 'Company'
FROM tails WHERE variant IS NOT NULL;

DROP VIEW IF EXISTS 'viewPilots';
CREATE VIEW viewPilots AS  
SELECT  pilot_id AS 'ID',  
        lastname AS 'Last Name',
        firstname AS 'First Name',
        company AS 'Company'
FROM pilots;

DROP VIEW IF EXISTS 'viewTotals';
CREATE VIEW viewTotals AS  
SELECT  printf('%02d',CAST(SUM(tblk) AS INT)/60)||':'||printf('%02d',CAST(SUM(tblk) AS INT)%60) AS 'TOTAL',  
        printf('%02d',CAST(SUM(tSPSE) AS INT)/60)||':'||printf('%02d',CAST(SUM(tSPSE) AS INT)%60) AS 'SP SE',
        printf('%02d',CAST(SUM(tSPME) AS INT)/60)||':'||printf('%02d',CAST(SUM(tSPME) AS INT)%60) AS 'SP ME',
        printf('%02d',CAST(SUM(tNIGHT) AS INT)/60)||':'||printf('%02d',CAST(SUM(tNIGHT) AS INT)%60) AS 'NIGHT',
        printf('%02d',CAST(SUM(tIFR) AS INT)/60)||':'||printf('%02d',CAST(SUM(tIFR) AS INT)%60) AS 'IFR',
        printf('%02d',CAST(SUM(tPIC) AS INT)/60)||':'||printf('%02d',CAST(SUM(tPIC) AS INT)%60) AS 'PIC',
        printf('%02d',CAST(SUM(tPICUS) AS INT)/60)||':'||printf('%02d',CAST(SUM(tPICUS) AS INT)%60) AS 'PICUS',
        printf('%02d',CAST(SUM(tSIC) AS INT)/60)||':'||printf('%02d',CAST(SUM(tSIC) AS INT)%60) AS 'SIC',
        printf('%02d',CAST(SUM(tDual) AS INT)/60)||':'||printf('%02d',CAST(SUM(tDual) AS INT)%60) AS 'DUAL',
        printf('%02d',CAST(SUM(tFI) AS INT)/60)||':'||printf('%02d',CAST(SUM(tFI) AS INT)%60) AS 'INSTRUCTOR',
        printf('%02d',CAST(SUM(tSIM) AS INT)/60)||':'||printf('%02d',CAST(SUM(tSIM) AS INT)%60) AS 'SIMULATOR',
        printf('%02d',CAST(SUM(tMP) AS INT)/60)||':'||printf('%02d',CAST(SUM(tMP) AS INT)%60) AS 'MultPilot',
        CAST(SUM(toDay) AS INT) AS 'TO Day',
        CAST(SUM(toNight) AS INT) AS 'TO Night',
        CAST(SUM(ldgDay) AS INT) AS 'LDG Day',
        CAST(SUM(ldgNight) AS INT) AS 'LDG Night'
FROM flights;

DROP VIEW IF EXISTS 'viewExport';
CREATE VIEW viewExport AS
SELECT  flight_id,
        doft as 'Date',
        dept AS 'Dept',
        printf('%02d',(tofb/60))||':'||printf('%02d',(tofb%60)) AS 'Time Out',
        dest AS 'Dest',
        printf('%02d',(tonb/60))||':'||printf('%02d',(tonb%60)) AS 'Time In ',
        CASE  WHEN variant IS NOT NULL THEN make||' '||model||'-'||variant  ELSE make||' '||model  END  AS 'Type',
        registration AS 'Registration',
        (SELECT printf('%02d',(tSPSE/60))||':'||printf('%02d',(tSPSE%60)) WHERE tSPSE IS NOT NULL) AS 'SP SE',
        (SELECT printf('%02d',(tSPME/60))||':'||printf('%02d',(tSPME%60)) WHERE tSPME IS NOT NULL) AS 'SP ME',
        (SELECT printf('%02d',(tMP/60))||':'||printf('%02d',(tMP%60)) WHERE tMP IS NOT NULL) AS 'MP',
        printf('%02d',(tblk/60))||':'||printf('%02d',(tblk%60)) AS 'Total',
        CASE  WHEN pilot_id = 1 THEN alias  ELSE lastname||', '||substr(firstname, 1, 1)||'.'  END  AS 'Name PIC',
        toDay AS 'Take-Off Day',
        ldgDay AS 'Landings Day',
        toNight AS 'Take-Off Night',
        ldgNight AS 'Landings Night',
        (SELECT printf('%02d',(tNight/60))||':'||printf('%02d',(tNight%60)) WHERE tNight IS NOT NULL)  AS 'Night',
        (SELECT printf('%02d',(tIFR/60))||':'||printf('%02d',(tIFR%60)) WHERE tIFR IS NOT NULL)  AS 'IFR',
        (SELECT printf('%02d',(tPIC/60))||':'||printf('%02d',(tPIC%60)) WHERE tPIC IS NOT NULL)  AS 'PIC',
        (SELECT printf('%02d',(tSIC/60))||':'||printf('%02d',(tSIC%60)) WHERE tSIC IS NOT NULL)  AS 'SIC',
        (SELECT printf('%02d',(tDual/60))||':'||printf('%02d',(tDual%60)) WHERE tDual IS NOT NULL)  AS 'Dual',
        (SELECT printf('%02d',(tFI/60))||':'||printf('%02d',(tFI%60)) WHERE tFI IS NOT NULL)  AS 'FI',
        null AS 'Sim Type',
        null AS 'Time of Session',
        remarks AS 'Remarks'
FROM flights
INNER JOIN pilots on flights.pic = pilots.pilot_id
INNER JOIN tails on flights.acft = tails.tail_id
UNION
SELECT (session_id * -1),
        date,
        null,  null,  null,  null,
        aircraftType,
        registration,
        null,  null,  null,
        'SIM',
        null,  null,  null,  null,  null,  null,  null,  null,  null,  null, null,
        deviceType,  printf('%02d',(totalTime/60))||':'||printf('%02d',(totalTime%60)),
        remarks
FROM simulators
ORDER BY date DESC;
