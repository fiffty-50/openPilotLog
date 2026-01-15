CREATE TABLE movement_events (
    movement_event_id INTEGER PRIMARY KEY AUTOINCREMENT,
    event_id INTEGER NOT NULL,
    airport_id INTEGER NOT NULL,
    is_landing INTEGER NOT NULL,
    is_night INTEGER NOT NULL,
    is_autoland INTEGER NOT NULL,

    CHECK (is_landing IN (0,1)),
    CHECK (is_night IN (0,1)),
    CHECK (is_autoland IN (0,1)),
    
    FOREIGN KEY (event_id) REFERENCES log_events(event_id) ON DELETE CASCADE,
    FOREIGN KEY (airport_id) REFERENCES airports(airport_id) ON DELETE RESTRICT
);