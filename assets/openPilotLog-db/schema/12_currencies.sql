CREATE TABLE currencies (
    currency_id INTEGER PRIMARY KEY AUTOINCREMENT,
    currency_type TEXT NOT NULL,
    currency_name TEXT NOT NULL,
    valid_from_jd INTEGER NOT NULL,
    valid_to_jd INTEGER NOT NULL,
    certificate_number TEXT,
    remarks TEXT,

    CHECK (currency_type IN ('LICENSE', 'MEDICAL', 'TYPE_RATING', 'INSTRUMENT_RATING', 'OTHER')),
    CHECK (valid_from_jd <= valid_to_jd)
);