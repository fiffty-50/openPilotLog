# Pilot Currencies

This table tracks pilot licenses, medical certificates, type ratings, and other qualifications with their validity periods.

## Fields

| Column | Type | Description |
| --- | --- | --- |
| 'currency_id' | INTEGER | Primary Key |
| 'currency_type' | TEXT NOT NULL | Type of credential (e.g., 'LICENSE', 'MEDICAL', 'TYPE_RATING', 'RATING', 'OTHER') |
| 'currency_name' | TEXT NOT NULL | Name of the credential (e.g., 'ATPL', 'JAA Class 1 Medical', 'B737', 'Night Rating') |
| 'valid_from_jd' | INTEGER NOT NULL | Julian day when credential becomes valid |
| 'valid_to_jd' | INTEGER NOT NULL | Julian day when credential expires |
| 'issuing_authority' | TEXT | optional - Authority that issued the credential (e.g., 'FAA', 'EASA', 'CASA') |
| 'certificate_number' | TEXT | optional - Certificate/license number for reference |
| 'remarks' | TEXT | optional |

## Notes

- `currency_type` is a controlled vocabulary to enable filtering by credential category
- Type ratings and endorsements may have different renewal requirements than the base license
- CHECK: `valid_from_jd <= valid_to_jd`
