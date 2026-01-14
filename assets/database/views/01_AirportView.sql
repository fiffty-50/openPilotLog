CREATE VIEW AirportView AS
SELECT 
    a.airport_id AS airport_id,
    MAX(CASE WHEN ac.code_type = 'IATA' THEN ac.airport_code ELSE NULL END) AS iata_code,
    MAX(CASE WHEN ac.code_type = 'ICAO' THEN ac.airport_code ELSE NULL END) AS icao_code,
	a.timezone_olson AS timezone,
	a.airport_name AS name
FROM airports a
LEFT JOIN airport_codes ac ON a.airport_id = ac.airport_id
GROUP BY a.airport_id, a.airport_name;