CREATE TABLE pilots (
    pilot_id INTEGER PRIMARY KEY AUTOINCREMENT,
    pilot_name TEXT NOT NULL UNIQUE,
    alias TEXT,
    employee_id TEXT,
    phone TEXT,
    email TEXT,
    remarks TEXT
);

CREATE INDEX idx_pilots_name ON pilots(pilot_name);