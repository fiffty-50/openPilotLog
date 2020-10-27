# openpilotlog
A FOSS pilot logbook written in C++(Qt5)


New folder and organizational structure
note: Database and settings stored in "data" folder in application directory

All database functionality now in one class

Using QSettings class instead of custom solution (storing settings in database)
settings table in database now obsolete

New class completionLists, provides lists for QCompleter

New class aircraft

Rework of New Aircraft Dialog (WIP)
