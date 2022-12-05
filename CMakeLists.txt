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

    ##  GUI
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
    # Verification
    src/gui/verification/userinput.h
    src/gui/verification/userinput.cpp
    src/gui/verification/timeinput.h
    src/gui/verification/timeinput.cpp
    src/gui/verification/airportinput.h
    src/gui/verification/airportinput.cpp
    src/gui/verification/pilotinput.h
    src/gui/verification/pilotinput.cpp
    src/gui/verification/completerprovider.h
    src/gui/verification/completerprovider.cpp
    src/gui/verification/tailinput.h
    src/gui/verification/tailinput.cpp


    # Classes
    src/classes/style.h
    src/classes/style.cpp
    src/classes/paths.h
    src/classes/paths.cpp
    src/classes/downloadhelper.h
    src/classes/downloadhelper.cpp
    src/classes/runguard.h
    src/classes/runguard.cpp
    src/classes/settings.h
    src/classes/settings.cpp
    src/classes/translator.h
    src/classes/translator.cpp
    src/classes/jsonhelper.h
    src/classes/jsonhelper.cpp
    src/classes/md5sum.h
    src/classes/md5sum.cpp

    # Namespaces
    src/functions/calc.h
    src/functions/calc.cpp
    src/functions/log.h
    src/functions/log.cpp
    src/functions/readcsv.h
    src/functions/statistics.h
    src/functions/statistics.cpp
    src/functions/datetime.h
    src/functions/datetime.cpp
    src/functions/time.h

    # Database
    src/database/database.h
    src/database/database.cpp
    src/database/row.h
    src/database/row.cpp
    src/database/dbsummary.h
    src/database/dbsummary.cpp
    src/database/databasecache.h
    src/database/databasecache.cpp

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
    src/testing/randomgenerator.h
    src/testing/randomgenerator.cpp
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
        ${app_icon_macos}
    )
else()
    add_executable(openPilotLog
        ${PROJECT_SOURCES}
        ${QM_FILES}
        ${app_icon_macos}
    )
endif()


set_target_properties(openPilotLog PROPERTIES
    MACOSX_BUNDLE TRUE
)
# The MACOSX_BUNDLE_ICON_FILE variable is added to the Info.plist
# generated by CMake. This variable contains the .icns file name,
# without the path.
set(MACOSX_BUNDLE_ICON_FILE icon_ios.icns)

# And the following tells CMake where to find and install the file itself.
set(app_icon_macos "${CMAKE_CURRENT_SOURCE_DIR}/assets/opl-icons/app/icon_ios.icns")
set_source_files_properties(${app_icon_macos} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

# target_link_libraries(openPilotLog PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Network OpenSSL::SSL)
target_link_libraries(openPilotLog PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Network)

install(TARGETS openPilotLog DESTINATION bin)
