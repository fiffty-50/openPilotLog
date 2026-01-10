CREATE TABLE simulators (
    sim_id INTEGER PRIMARY KEY AUTOINCREMENT,
    event_id INTEGER NOT NULL,
    sim_type TEXT NOT NULL,
    aircraft_type_id INTEGER,
    instructor_id INTEGER,
    second_pilot_id INTEGER,
    duration_ms INTEGER NOT NULL,
    FOREIGN KEY (event_id) REFERENCES log_events(event_id) ON DELETE RESTRICT,
    FOREIGN KEY (aircraft_type_id) REFERENCES aircraft_types(aircraft_type_id) ON DELETE SET NULL,
    FOREIGN KEY (instructor_id) REFERENCES pilots(pilot_id) ON DELETE RESTRICT,
    FOREIGN KEY (second_pilot_id) REFERENCES pilots(pilot_id) ON DELETE SET NULL,
    CHECK (duration_ms > 0)
);

CREATE INDEX idx_simulators_event_id ON simulators(event_id);
CREATE INDEX idx_simulators_aircraft_type ON simulators(aircraft_type_id);
CREATE INDEX idx_simulators_instructor ON simulators(instructor_id);