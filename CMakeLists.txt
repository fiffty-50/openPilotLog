cmake_minimum_required(VERSION 3.20)

project(openPilotLog LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_MIN_VERSION "6.2")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_USE_QSTRINGBUILDER")
MESSAGE ("Enabling QStringBuilder")

#find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Sql Network REQUIRED)
# find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Sql Network LinguistTools REQUIRED)
# find_package(OpenSSL REQUIRED) # Pending testing
# find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Sql Network REQUIRED)
find_package(Qt6 COMPONENTS Widgets Sql Network REQUIRED)
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
    src/gui/dialogues/airportentryeditdialog.h
    src/gui/dialogues/airportentryeditdialog.cpp
    src/gui/dialogues/pilotentryeditdialog.h
    src/gui/dialogues/pilotentryeditdialog.cpp
    src/gui/dialogues/tailentryeditdialog.h
    src/gui/dialogues/tailentryeditdialog.cpp
    src/gui/dialogues/simentryeditdialog.h
    src/gui/dialogues/simentryeditdialog.cpp
    src/gui/dialogues/exporttocsvdialog.h
    src/gui/dialogues/exporttocsvdialog.cpp
    src/gui/dialogues/exporttocsvdialog.ui

    src/gui/dialogues/entryeditdialog.h
    src/gui/dialogues/entryeditdialog.cpp
    src/gui/dialogues/aircraftentryeditdialog.h
    src/gui/dialogues/aircraftentryeditdialog.cpp

    src/gui/dialogues/flightentryeditui.h
    src/gui/dialogues/flightentryeditui.cpp
    src/gui/dialogues/flightlogentryeditdialog.h
    src/gui/dialogues/flightlogentryeditdialog.cpp

    # Widgets
    src/gui/widgets/homewidget.h
    src/gui/widgets/homewidget.cpp
    src/gui/widgets/homewidget.ui
    src/gui/widgets/totalswidget.h
    src/gui/widgets/totalswidget.cpp
    src/gui/widgets/totalswidget.ui
    src/gui/widgets/backupwidget.h
    src/gui/widgets/backupwidget.cpp
    src/gui/widgets/backupwidget.ui
    src/gui/widgets/debugwidget.h
    src/gui/widgets/debugwidget.cpp
    src/gui/widgets/debugwidget.ui

    src/gui/widgets/settingswidget.h
    src/gui/widgets/settingswidget.cpp
    src/gui/widgets/settingswidget.ui

    src/gui/widgets/tableeditwidget.h
    src/gui/widgets/tableeditwidget.cpp
    src/gui/widgets/pilottableeditwidget.h
    src/gui/widgets/pilottableeditwidget.cpp
    src/gui/widgets/tailtableeditwidget.h
    src/gui/widgets/tailtableeditwidget.cpp
    src/gui/widgets/airporttableeditwidget.h
    src/gui/widgets/airporttableeditwidget.cpp
    src/gui/widgets/logbooktableeditwidget.h
    src/gui/widgets/logbooktableeditwidget.cpp
    src/gui/widgets/aircrafttableeditwidget.h
    src/gui/widgets/aircrafttableeditwidget.cpp

    src/gui/widgets/databaseeditwidget.h
    src/gui/widgets/databaseeditwidget.cpp
    src/gui/widgets/flightsegmenteditorwidget.h
    src/gui/widgets/flightsegmenteditorwidget.cpp

    # Verification
    src/gui/comboboxes/dbselectioncombobox.h
    src/gui/comboboxes/dbselectioncombobox.cpp
    src/gui/comboboxes/tailselectionbox.h
    src/gui/comboboxes/tailselectionbox.cpp
    src/gui/comboboxes/pilotselectionbox.h
    src/gui/comboboxes/pilotselectionbox.cpp
    src/gui/comboboxes/airportselectionbox.h
    src/gui/comboboxes/airportselectionbox.cpp

    src/gui/verification/diacriticignoringstringlistmodel.h
    src/gui/verification/diacriticignoringcompleter.h
    src/gui/widgets/currencywidget.h
    src/gui/widgets/currencywidget.cpp
    src/gui/verification/entrybuilder.h

    src/gui/verification/flightdatabuilder.h
    src/gui/verification/flightdatabuilder.cpp
    src/gui/verification/flightsegmentbuilder.h
    src/gui/verification/flightsegmentbuilder.cpp

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
    src/classes/time.h
    src/classes/easaftl.h
    src/classes/easaftl.cpp
    src/classes/date.h
    src/gui/styleddelegates/styleddatedelegate.h
    src/gui/styleddelegates/styleddatedelegate.cpp
    src/gui/styleddelegates/styledtimedelegate.h
    src/gui/styleddelegates/styledtimedelegate.cpp
    src/gui/styleddelegates/styledpilotdelegate.h
    src/gui/styleddelegates/styledpilotdelegate.cpp
    src/gui/styleddelegates/styledregistrationdelegate.h
    src/gui/styleddelegates/styledregistrationdelegate.cpp
    src/gui/styleddelegates/styledtypedelegate.h
    src/gui/styleddelegates/styledtypedelegate.cpp
    src/gui/styleddelegates/styledenginecountdelegate.h
    src/gui/styleddelegates/styledenginecountdelegate.cpp

    # Database Entries
    src/database/aircraftentry.h
    src/database/aircraftentry.cpp
    src/database/tailentry.h
    src/database/tailentry.cpp
    src/database/airportentry.h
    src/database/airportentry.cpp
    src/database/pilotentry.h
    src/database/pilotentry.cpp
    src/database/airportcodeentry.h
    src/database/airportcodeentry.cpp
    src/database/logentry.h
    src/database/logentry.cpp
    src/database/flightlogentry.h
    src/database/flightlogentry.cpp
    src/database/simlogentry.h
    src/database/simlogentry.cpp
    src/database/movemententry.h
    src/database/movemententry.cpp
    src/database/flightsegmententry.h
    src/database/flightsegmententry.cpp
    src/database/approachentry.h
    src/database/approachentry.cpp

    src/database/flightdata.h
    src/database/flightdata.cpp

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

    # Math
    src/calc/greatcircletrack.h
    src/calc/greatcircletrack.cpp
    src/calc/nighttime.h
    src/calc/nighttime.cpp
    src/calc/spawrapper.h
    src/calc/spa.h # external
    src/calc/spa.c # external



    # Database
    src/database/database.h
    src/database/database.cpp
    src/database/databasesetup.h
    src/database/databasesetup.cpp
    src/database/row.h
    src/database/row.cpp
    src/database/dbsummary.h
    src/database/dbsummary.cpp
    src/database/databasecache.h
    src/database/databasecache.cpp

    src/database/views/logbookviewinfo.h
    src/database/airportinfo.h
    src/database/airportinfo.cpp
    src/database/pilotinfo.h
    src/database/pilotinfo.cpp
    src/database/tailregistrationsinfo.h
    src/database/tailregistrationsinfo.cpp
    src/database/aircrafttypesinfo.h
    src/database/aircrafttypesinfo.cpp
    src/database/airportgeographicalinfo.h
    src/database/airportgeographicalinfo.cpp

    # Ressources
    assets/icons.qrc
    assets/database.qrc
    assets/themes/stylesheets/breeze/breeze.qrc
    assets/themes/stylesheets/qdarkstyle/qdarkstyle.qrc

    # network
    src/network/flightawareflightdata.h
    src/network/flightawarequery.h
    src/network/flightawarequery.cpp
    src/network/flightawarejsonparser.h
    src/network/flightawarejsonparser.cpp


    # Testing / Debug
    #src/testing/importCrewlounge/importcrewlounge.h
    #src/testing/importCrewlounge/importcrewlounge.cpp
    #src/testing/importCrewlounge/processpilots.h
    #src/testing/importCrewlounge/processpilots.cpp
    #src/testing/importCrewlounge/processaircraft.h
    #src/testing/importCrewlounge/processaircraft.cpp
    #src/testing/importCrewlounge/processflights.h
    #src/testing/importCrewlounge/processflights.cpp
    src/testing/atimer.h
    src/testing/atimer.cpp
    #src/testing/randomgenerator.h
    #src/testing/randomgenerator.cpp

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

# set the win32 entry point to avoid terminal popping up
if(WIN32)
    MESSAGE("Building for WIN32")
    if(QT_VERSION_MAJOR GREATER_EQUAL 6)
        qt_add_executable(openPilotLog WIN32
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
else()
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
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
