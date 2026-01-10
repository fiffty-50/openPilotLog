# Database

The Database layout and schema is detailed and maintained in a seperate repository (tbd - link here as subtree or submodule). This folder contains a local copy of the files needed to initialise the database from scratch

## Schema
This folder contains the SQL CREATE statements for the tables and indices

## Views
This folder contains the SQL CREATE statements for the database views used in the GUI

## Templates
This folder contains data in JSON files which is used to fill the template tables in the database (Airports and Aircraft Types).

## Sample Database

This folder also contains a file `logbook.db`, which is a sample databes filled with data for testing and debugging the application.

In order to use it with the application, it needs to be put in the
[data location](https://doc.qt.io/qt-5/qstandardpaths.html#StandardLocation-enum) of your device, for example:

`~/.local/share/opl/openPilotLog` or `C:/Users/<USER>/AppData/Local/opl`
