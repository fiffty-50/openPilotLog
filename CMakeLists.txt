cmake_minimum_required(VERSION 3.0)

project(openPilotLog LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_MIN_VERSION "5.5.1")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_USE_QSTRINGBUILDER")
MESSAGE ("Enabling QStringBuilder")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Sql Network LinguistTools REQUIRED)
# find_package(OpenSSL REQUIRED) # Pending testing

set(PROJECT_SOURCES

    main.cpp
    src/opl.h
    src/opl.cpp

    # GUI
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    # Dialogs
    src/gui/dialogues/firstrundialog.h
    src/gui/dialogues/firstrundialog.cpp
    src/gui/dialogues/firstrundialog.ui
    src/gui/dialogues/newairportdialog.h
    src/gui/dialogues/newairportdialog.cpp
    src/gui/dialogues/newairportdialog.ui
    src/gui/dialogues/newflightdialog.h
    src/gui/dialogues/newflightdialog.cpp
    src/gui/dialogues/newflightdialog.ui
    src/gui/dialogues/newpilotdialog.h
    src/gui/dialogues/newpilotdialog.cpp
    src/gui/dialogues/newpilot.ui
    src/gui/dialogues/newtaildialog.h
    src/gui/dialogues/newtaildialog.cpp
    src/gui/dialogues/newtail.ui
    src/gui/dialogues/newsimdialog.h
    src/gui/dialogues/newsimdialog.cpp
    src/gui/dialogues/newsimdialog.ui

    # Widgets
    src/gui/widgets/aircraftwidget.h
    src/gui/widgets/aircraftwidget.cpp
    src/gui/widgets/aircraftwidget.ui
    src/gui/widgets/airportwidget.h
    src/gui/widgets/airportwidget.cpp
    src/gui/widgets/airportwidget.ui
    src/gui/widgets/backupwidget.h
    src/gui/widgets/backupwidget.cpp
    src/gui/widgets/backupwidget.ui
    src/gui/widgets/debugwidget.h
    src/gui/widgets/debugwidget.cpp
    src/gui/widgets/debugwidget.ui
    src/gui/widgets/homewidget.h
    src/gui/widgets/homewidget.cpp
    src/gui/widgets/homewidget.ui
    src/gui/widgets/logbookwidget.h
    src/gui/widgets/logbookwidget.cpp
    src/gui/widgets/logbookwidget.ui
    src/gui/widgets/pilotswidget.h
    src/gui/widgets/pilotswidget.cpp
    src/gui/widgets/pilotswidget.ui
    src/gui/widgets/settingswidget.cpp
    src/gui/widgets/settingswidget.h
    src/gui/widgets/settingswidget.ui


    # Classes
    src/classes/acompletiondata.h
    src/classes/acompletiondata.cpp
    src/classes/acurrencyentry.h
    src/classes/acurrencyentry.cpp
    src/classes/astyle.h
    src/classes/astyle.cpp
    src/classes/astandardpaths.h
    src/classes/astandardpaths.cpp
    src/classes/aaircraftentry.h
    src/classes/aaircraftentry.cpp
    src/classes/adownload.h
    src/classes/adownload.cpp
    src/classes/aentry.h
    src/classes/aentry.cpp
    src/classes/aflightentry.h
    src/classes/aflightentry.cpp
    src/classes/apilotentry.h
    src/classes/apilotentry.cpp
    src/classes/arunguard.h
    src/classes/arunguard.cpp
    src/classes/asettings.h
    src/classes/asettings.cpp
    src/classes/atailentry.h
    src/classes/atailentry.cpp
    src/classes/atranslator.h
    src/classes/atranslator.cpp
    src/classes/ajson.h
    src/classes/ajson.cpp
    src/classes/ahash.h
    src/classes/ahash.cpp
    src/classes/asimulatorentry.h
    src/classes/asimulatorentry.cpp

    # Namespaces
    src/functions/acalc.h
    src/functions/acalc.cpp
    src/functions/adatetime.h
    src/functions/alog.h
    src/functions/alog.cpp
    src/functions/areadcsv.h
    src/functions/areadcsv.cpp
    src/functions/astat.h
    src/functions/astat.cpp
    src/functions/adate.h
    src/functions/adate.cpp
    src/functions/atime.h

    # Database
    src/database/adatabasetypes.h
    src/database/adatabase.h
    src/database/adatabase.cpp

    # Ressources
    assets/icons.qrc
    assets/database/templates.qrc
    assets/themes/stylesheets/breeze/breeze.qrc
    assets/themes/stylesheets/qdarkstyle/qdarkstyle.qrc

    # Testing / Debug
    src/testing/importCrewlounge/importcrewlounge.h
    src/testing/importCrewlounge/importcrewlounge.cpp
    src/testing/importCrewlounge/processpilots.h
    src/testing/importCrewlounge/processpilots.cpp
    src/testing/importCrewlounge/processaircraft.h
    src/testing/importCrewlounge/processaircraft.cpp
    src/testing/importCrewlounge/processflights.h
    src/testing/importCrewlounge/processflights.cpp
    src/testing/atimer.h
    src/testing/atimer.cpp
)

# This is currently a bit buggy, see
# https://bugreports.qt.io/browse/QTBUG-41736
# https://bugreports.qt.io/browse/QTBUG-76410
# Before working on the translations themselves, check for updates and consider not updating for
# every build due to danger of loss of translations...
#if (Qt5Widgets_FOUND)
#    if (Qt5Widgets_VERSION VERSION_LESS 5.15.0)
#        message("Translations are available for Qt5 version >= 5.15")
#    else()
#        message("Qt > 5.15 detected. Enabling translations.")
#        set(TS_FILES
#            l10n/openpilotlog_en.ts
#            l10n/openpilotlog_de.ts
#            l10n/openpilotlog_es.ts
#        )
#        set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "l10n")
#        qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
#    endif(Qt5Widgets_VERSION VERSION_LESS 5.15.0)
#endif(Qt5Widgets_FOUND)

# Disable Debug output for Release Build
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Build type: Debug")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Build type: Release")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(openPilotLog
        ${PROJECT_SOURCES}
        ${QM_FILES}
    )
else()
    add_executable(openPilotLog
        ${PROJECT_SOURCES}
        ${QM_FILES}
    )
endif()

# target_link_libraries(openPilotLog PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Network OpenSSL::SSL)
target_link_libraries(openPilotLog PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Network)

install(TARGETS openPilotLog DESTINATION bin)
