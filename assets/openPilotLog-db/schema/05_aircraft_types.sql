CREATE TABLE aircraft_types (
    aircraft_type_id INTEGER PRIMARY KEY AUTOINCREMENT,
    make TEXT NOT NULL,
    model TEXT NOT NULL,
    variant TEXT,
    icao_designator TEXT,
    engine_type TEXT NOT NULL,
    is_multi_engine INTEGER NOT NULL,
    class TEXT NOT NULL,
    sub_class TEXT NOT NULL,
    is_multi_pilot INTEGER NOT NULL,
    wake_category TEXT,
    type_rating TEXT,
    remarks TEXT,

    CHECK (is_multi_engine IN (0,1)),
    CHECK (is_multi_pilot IN (0,1)),
    CHECK (engine_type IN ('UNPOWERED', 'PISTON', 'TURBOPROP', 'JET', 'ELECTRIC', 'OTHER')),
    CHECK (class IN ('AEROPLANE', 'ROTORCRAFT', 'SAILPLANE', 'BALLOON', 'AIRSHIP', 'OTHER')),
    CHECK (sub_class IN ('LAND', 'SEA'))
);
