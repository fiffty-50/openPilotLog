CREATE TABLE log_events (
    event_id INTEGER PRIMARY KEY AUTOINCREMENT,
    event_date_jd INTEGER NOT NULL,
    event_type TEXT NOT NULL,
    remarks TEXT,
    CHECK (event_type IN ('FLT', 'SIM'))
);
CREATE INDEX idx_log_events_event_date ON log_events(event_date_jd);
CREATE INDEX idx_log_events_event_type ON log_events(event_type);