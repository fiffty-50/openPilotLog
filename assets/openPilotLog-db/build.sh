#!/bin/bash
# Build script for openPilotLog database

DB_FILE="openPilotLog.db"
BACKUP_FILE="openPilotLog_$(date +%Y%m%d%H%M%S).db"

# Move the database file if it already exists
if [ -f "$DB_FILE" ]; then
    echo "Moving existing database file to: $BACKUP_FILE"
    mv "$DB_FILE" "$BACKUP_FILE"
fi

# Create the database and apply all SQL files
echo "Creating Database..."
for sql_file in schema/*.sql; do
    echo "Applying $sql_file..."
    sqlite3 "$DB_FILE" < "$sql_file"
    echo "$sql_file applied successfully."
done

echo "Creating Views..."
# Create the views
for view_file in views/*.sql; do
    echo "Applying $view_file..."
    sqlite3 "$DB_FILE" < "$view_file"
    echo "$sql_file applied successfully."
done

# Final message
echo "Database build complete: $DB_FILE"
