CREATE TABLE airport_codes (
    airport_code_id INTEGER PRIMARY KEY AUTOINCREMENT,
    airport_id INTEGER NOT NULL,
    airport_code TEXT NOT NULL,
    code_type TEXT NOT NULL,
    valid_from_jd INTEGER NOT NULL,
    valid_to_jd INTEGER,

    FOREIGN KEY (airport_id) REFERENCES airports(airport_id),

    CHECK (code_type IN ('IATA', 'ICAO', 'OTHER')),
    CHECK (valid_to_jd IS NULL OR valid_to_jd > valid_from_jd),
    UNIQUE (airport_id, airport_code, valid_from_jd),
    UNIQUE (airport_code, code_type, valid_from_jd)
);