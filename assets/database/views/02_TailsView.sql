CREATE VIEW TailsView AS
SELECT
  ta.tail_id,
  ta.registration,
  CASE
    WHEN ty.variant IS NULL
      THEN ty.make || ' ' || ty.model
    ELSE
      ty.make || ' ' || ty.model || '-' || ty.variant
  END AS Type,
  ta.company
FROM aircraft_tails AS ta
INNER JOIN aircraft_types AS ty
USING (aircraft_type_id);