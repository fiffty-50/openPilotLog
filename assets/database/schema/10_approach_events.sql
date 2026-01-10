CREATE TABLE approach_events (
    approach_event_id INTEGER PRIMARY KEY AUTOINCREMENT,
    event_id INTEGER NOT NULL,
    airport_id INTEGER NOT NULL,
    approach_type TEXT NOT NULL,

    FOREIGN KEY (event_id) REFERENCES log_events(event_id) ON DELETE CASCADE
);

CREATE INDEX idx_approach_events_event_id ON approach_events(event_id);