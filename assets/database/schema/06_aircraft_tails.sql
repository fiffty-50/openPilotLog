CREATE TABLE aircraft_tails (
    tail_id INTEGER PRIMARY KEY AUTOINCREMENT,
    aircraft_type_id INTEGER NOT NULL,
    registration TEXT NOT NULL,
    in_service_jd INTEGER NOT NULL,
    out_of_service_jd INTEGER,
    company TEXT,
    remarks TEXT,
    FOREIGN KEY (aircraft_type_id) REFERENCES aircraft_types(aircraft_type_id) ON DELETE RESTRICT,
    CHECK (out_of_service_jd IS NULL OR out_of_service_jd > in_service_jd),
    UNIQUE (registration, in_service_jd)
);

CREATE INDEX idx_tails_registration ON aircraft_tails(registration);
CREATE INDEX idx_tails_aircraft_type ON aircraft_tails(aircraft_type_id);
CREATE INDEX idx_tails_in_service ON aircraft_tails(in_service_jd);