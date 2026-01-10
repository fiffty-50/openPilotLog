CREATE TABLE airports (
    airport_id INTEGER PRIMARY KEY AUTOINCREMENT,
    airport_name TEXT NOT NULL,
    latitude REAL NOT NULL,
    longitude REAL NOT NULL,
    elevation_ft INTEGER,
    timezone_olson TEXT,

    CHECK (latitude >= -90 AND latitude <= 90),
    CHECK (longitude >= -180 AND longitude <= 180)
)