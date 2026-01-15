CREATE TABLE log_events (
    event_id INTEGER PRIMARY KEY AUTOINCREMENT,
    event_date_jd INTEGER NOT NULL,
    event_type TEXT NOT NULL,
    remarks TEXT,

    CHECK (event_type IN ('FLT', 'SIM'))
);