CREATE TABLE flight_segments (
    segment_id INTEGER PRIMARY KEY AUTOINCREMENT,
    flight_id INTEGER NOT NULL,
    start_ms INTEGER NOT NULL,
    end_ms INTEGER NOT NULL,
    is_ifr INTEGER NOT NULL,
    is_simulated_ifr INTEGER NOT NULL,
    is_night INTEGER NOT NULL,
    is_multi_pilot INTEGER,
    pilot_function TEXT NOT NULL,

    CHECK (is_ifr IN (0,1)),
    CHECK (is_simulated_ifr IN (0,1)),
    CHECK (is_night IN (0,1)),
    CHECK (is_multi_pilot IN (0,1)),
    CHECK (pilot_function IN ('PIC', 'PICUS', 'SIC', 'DUAL', 'INSTRUCTOR')),

    FOREIGN KEY (flight_id) REFERENCES flights(flight_id) ON DELETE CASCADE
);