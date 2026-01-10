CREATE TABLE flights (
    flight_id INTEGER PRIMARY KEY AUTOINCREMENT,
    event_id INTEGER NOT NULL UNIQUE,
    departure INTEGER NOT NULL,
    destination INTEGER NOT NULL,
    tofb_ms INTEGER NOT NULL,
    tonb_ms INTEGER NOT NULL,
    pic_pilot_id INTEGER NOT NULL,
    second_pilot_id INTEGER,
    third_pilot_id INTEGER,
    fourth_pilot_id INTEGER,
    tail INTEGER NOT NULL,
    flight_number TEXT,
    FOREIGN KEY (event_id) REFERENCES log_events(event_id) ON DELETE RESTRICT,
    FOREIGN KEY (departure) REFERENCES airports(airport_id) ON DELETE RESTRICT,
    FOREIGN KEY (destination) REFERENCES airports(airport_id) ON DELETE RESTRICT,
    FOREIGN KEY (pic_pilot_id) REFERENCES pilots(pilot_id) ON DELETE RESTRICT,
    FOREIGN KEY (tail) REFERENCES tails(tail_id) ON DELETE RESTRICT
);

CREATE INDEX idx_flights_event_id ON flights(event_id);
CREATE INDEX idx_flights_tail ON flights(tail);
CREATE INDEX idx_flights_departure ON flights(departure);
CREATE INDEX idx_flights_destination ON flights(destination);
CREATE INDEX idx_flights_pic ON flights(pic_pilot_id);
CREATE INDEX idx_flights_tail_event ON flights(tail, event_id);
