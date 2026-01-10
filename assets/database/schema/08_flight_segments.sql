CREATE TABLE flight_segments (
    segment_id INTEGER PRIMARY KEY AUTOINCREMENT,
    flight_id INTEGER NOT NULL,
    start_ms INTEGER NOT NULL,
    end_ms INTEGER NOT NULL,
    is_ifr INTEGER NOT NULL,
    is_simulated_ifr INTEGER NOT NULL,
    is_night INTEGER NOT NULL,
    pilot_function TEXT NOT NULL,
    FOREIGN KEY (flight_id) REFERENCES flights(flight_id) ON DELETE CASCADE,
    CHECK (is_ifr IN (0,1)),
    CHECK (is_simulated_ifr IN (0,1)),
    CHECK (is_night IN (0,1)),
    CHECK (pilot_function IN ('PIC', 'PICUS', 'SIC', 'DUAL', 'INSTRUCTOR'))
);

CREATE INDEX idx_flight_segments_flight_id ON flight_segments(flight_id);
CREATE INDEX idx_flight_segments_pilot_function ON flight_segments(pilot_function);
CREATE INDEX idx_flight_segments_function_flight ON flight_segments(pilot_function, flight_id);