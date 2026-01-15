CREATE TABLE simulators (
    sim_id INTEGER PRIMARY KEY AUTOINCREMENT,
    event_id INTEGER NOT NULL,
    sim_type TEXT NOT NULL,
    aircraft_type_id INTEGER,
    instructor_id INTEGER,
    second_pilot_id INTEGER,
    duration_ms INTEGER NOT NULL,

    CHECK (duration_ms > 0),

    FOREIGN KEY (event_id) REFERENCES log_events(event_id) ON DELETE RESTRICT,
    FOREIGN KEY (aircraft_type_id) REFERENCES aircraft_types(aircraft_type_id) ON DELETE SET NULL,
    FOREIGN KEY (instructor_id) REFERENCES pilots(pilot_id) ON DELETE RESTRICT,
    FOREIGN KEY (second_pilot_id) REFERENCES pilots(pilot_id) ON DELETE SET NULL
);