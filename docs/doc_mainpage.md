# Project Overview {#mainpage}


# Technical Overview

- Framework: Qt5 (C++)

- Database: sqlite3

This program is suppposed to replace a paper logbook with an electronic version. As a first step, I want to focus on the basics, and then add 'nice-to-haves' later.
These are the guidelines for this project:

- Speed: openPilotLog is supposed to be fast, lean and efficient. I have settled on using the Qt framework for the gui and code, as well as sqlite for the database.
- Cross-Platform Support: Linux, Windows and Mac (and maybe down the road iOS/Android) should be supported. Qt provides this cross-platform compatibility.
- Local: At the moment, I am not planning on implementing a cloud-sync solution. The database is local, on your machine, and moving it around should be as easy as copying it onto a USB stick. While I am not opposed to cloud syncing functionality, I want the application to be independent of external services.
- Free: This application is and always will be free to use, distribute and modify. I have used commercial solutions before and have been disappointed when a 'lifetime' license expired because the company needs to support their development and cloud infrastructure. That's fair enough, but I believe keeping a logbook of your flights is not rocket science and you should not have to pay a monthly subscription for that.
- This is an open source project. Feel free to copy, modify or distritbute it under the GPL v3.


# Compiling the project

The most straight forward way is to get a copy of [Qt](https://www.qt.io/download) and compile the project using either cmake or QtCreator and qmake. The `CMakeList.txt` and `.pro` files are included in the repository. CMake is recommended and will be the standard going forward.

On first launch, you will run through a setup wizard for the database. If you want to test the program with a sample database, use the `Backup` functionality. Select `Restore external backup` from the GUI and import the file `logbook.db` from the assets folder in the repository.

## Known issues

- The setup wizard for the database requires openSSL to download the most recent data. In order for this to work on Windows, you need to place the [openSSL dll's](https://wiki.openssl.org/index.php/Binaries) in the application directory. On Linux, this is normally not an issue. If you are running into problems there, try installing the `openssl-dev` package or similar from your distribution's package manager.

- When compiling with CMake, instead of updating the translations, there is a bug that might instead delete them. See [here](https://bugreports.qt.io/browse/QTBUG-41736)
  and [here](https://bugreports.qt.io/browse/QTBUG-76410). Long story short, make sure to use a version of CMake that is 3.16 or earlier, or 3.19.2 or later if you want
  to use localisations other than English (once they're implemented).

# Implementation Overview

Keeping a logbook of flights is a quintessential database task. This program could thus be seen as a user-friendly front-end to a database. The database is a sqlite3-Database, which is described in detail on the [Database](https://github.com/fiffty-50/openpilotlog/wiki/Database-Layout-and-Description) Wiki Page.

Access to the database is provided by the ADatabase Class, which is responsible for managing the database connection and creating and executing queries. Data is retreived from the database in form of AEntry objects. The AEntry class and its subclasses represent 'lines' in a database table. Internally, they are structs holding a [QMap<QVariant, QVariant>](https://doc.qt.io/qt-5/qmap.html) with the key being the column name and the value its value. These Objects are then parsed by the different widgets and dialogues to read or write user-provided data.

The database contents are displayed to the user in the different widgets. **Flights** are displayed in the LogbookWidget, **Pilots** in the PilotsWidget and **Aircraft** in the AircraftWidget. These widgets also give convenient access to the Dialogues (NewFlightDialog, NewPilotDialog, NewTailDialog), enabling editing, adding or removing entries of their respective categories.

Widget class Elements of the User Interface dispay data from the database in a [QTableView](https://doc.qt.io/qt-5/qtableview.html) with a [QSqlQueryModel](https://doc.qt.io/qt-5.12/qsqlquerymodel.html). Dialog class UI Elements receive and return AEntry objects, which are read and written to and from the database via the ADatabase class.

# Settings

The AStandardPaths class is responsible for creating cross-platform compatible directory structures based on [QStandardPaths](https://doc.qt.io/qt-5/qstandardpaths.html). Settings are stored in a `.ini` file at a standardized location. The Settings widget enables the user to adjust various settings, which are stored and accessed via the ASettings class, based on the [QSettings](https://doc.qt.io/qt-5/qsettings.html) interface.

# Import / Export

The BackupWidget enables creating and restoring backup copies of the database. 



