# Welcome to the openpilotlog wiki!

On this wiki you will find general information about different aspects of the project.


## Contribute to the project

If you want to contribute to the project, get in touch here [here](mailto:felix.turo@gmail.com?subject=[GitHub]%20Contributing%20to%20openPilotLog)

## Documentation

For code documentation head over to [doxygen](https://fiffty-50.github.io/openpilotlog/html/index.html)

You can also find our [code style conventions](https://github.com/fiffty-50/openpilotlog/wiki/Coding-Style) in this wiki.

A general description of the database layout can be found [here](https://github.com/fiffty-50/openpilotlog/wiki/Database-Layout-and-Description)

# Project Overview

Code: C++ (Qt5)

Database: sqlite3

This program is suppposed to replace a paper logbook with an electronic version. As a first step, I want to focus on the basics, and then add 'nice-to-haves' later.
These are the guidelines for this project:
- Speed: openPilotLog is supposed to be fast, lean and efficient. I have settled on using the Qt framework for the gui and code, as well as sqlite for the database.
- Cross-Platform Support: I am primarily using Linux, but Windows and Mac (and maybe down the road iOS/Android) should also be supported. Qt provides this cross-platform compatibility.
- Local: At the moment, I am not planning on implementing a cloud-sync solution. The database is local, on your machine, and moving it around should be as easy as copying it onto a USB stick. While I am not opposed to cloud syncing functionality, I want the application to be independent of external services.
- Free: This application is and always will be free to use, distribute and modify. I have used commercial solutions before and have been disappointed when a 'lifetime' license expired because the company needs to support their development and cloud infrastructure. That's fair enough, but I believe keeping a logbook of your flights is not rocket science and you should not have to pay a monthly subscription for that. 



# The state of the project - Should I use this?

Clear answer for the time being - Don't use it as your primary logbook. This is an early development stage and functionality, layout of the GUI etc. might change drastically. This could lead to database incompatibilies with earlier versions. For this reason, I am not providing a release for now. If you would like to test it, you'll have to compile the project yourself.


# Compiling the project

The most straight forward way is to get a copy of [Qt](https://www.qt.io/download) and compile the project using QtCreator and qmake, the PRO file is included in this repository. 

On first launch, the database will be created. If you want to test the program with a sample database, copy `logbook.db` from the assets folder into your application directory.

If you want to start a fresh logbook, you can use the debugwidget to create a new database from scratch.