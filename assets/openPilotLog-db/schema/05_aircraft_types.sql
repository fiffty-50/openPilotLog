CREATE TABLE aircraft_types (
    aircraft_type_id INTEGER PRIMARY KEY AUTOINCREMENT,
    make TEXT NOT NULL,
    model TEXT NOT NULL,
    variant TEXT,
    icao_designator TEXT,
    engine_type TEXT,
    engine_count INTEGER NOT NULL,
    class TEXT,
    sub_class TEXT,
    is_multi_pilot INTEGER,
    wake_category TEXT,
    type_rating TEXT,
    remarks TEXT,

    CHECK (engine_count >= 0),
    CHECK (is_multi_pilot IN (0,1)),
    CHECK (engine_type IN ('UNPOWERED', 'PISTON', 'TURBOPROP', 'JET', 'ELECTRIC', 'OTHER')),
    CHECK (class IN ('AEROPLANE', 'HELICOPTER', 'GLIDER', 'ROTORCRAFT', 'DRONE', 'OTHER')),
    CHECK (sub_class IN ('LAND', 'SEA', 'AMPHIBIAN'))
);