CREATE TABLE aircraft_tails (
    tail_id INTEGER PRIMARY KEY AUTOINCREMENT,
    aircraft_type_id INTEGER NOT NULL,
    registration TEXT NOT NULL,
    in_service_jd INTEGER NOT NULL,
    out_of_service_jd INTEGER,
    company TEXT,
    remarks TEXT,
    CHECK (out_of_service_jd IS NULL OR out_of_service_jd > in_service_jd),
    UNIQUE (registration, in_service_jd),

    FOREIGN KEY (aircraft_type_id) REFERENCES aircraft_types(aircraft_type_id) ON DELETE RESTRICT
);