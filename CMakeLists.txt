cmake_minimum_required(VERSION 3.5)

project(openPilotLog LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Sql Network REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    src/classes/astyle.cpp
    src/classes/astandardpaths.cpp
    src/classes/aaircraftentry.cpp
    src/classes/adownload.cpp
    src/classes/aentry.cpp
    src/classes/aflightentry.cpp
    src/classes/apilotentry.cpp
    src/classes/arunguard.cpp
    src/classes/asettings.cpp
    src/classes/atailentry.cpp
    src/database/adatabase.cpp
    src/database/adatabasesetup.cpp
    src/functions/acalc.cpp
    src/functions/areadcsv.cpp
    src/functions/astat.cpp
    src/gui/dialogues/firstrundialog.cpp
    src/gui/dialogues/newflightdialog.cpp
    src/gui/dialogues/newpilotdialog.cpp
    src/gui/dialogues/newtaildialog.cpp
    src/gui/widgets/aircraftwidget.cpp
    src/gui/widgets/debugwidget.cpp
    src/gui/widgets/homewidget.cpp
    src/gui/widgets/logbookwidget.cpp
    src/gui/widgets/pilotswidget.cpp
    src/gui/widgets/settingswidget.cpp
    src/gui/widgets/totalswidget.cpp
    src/testing/abenchmark.cpp
    src/testing/atimer.cpp
    
    mainwindow.h
    src/classes/astyle.h
    src/classes/astandardpaths.h
    src/classes/aaircraftentry.h
    src/classes/adownload.h
    src/classes/aentry.h
    src/classes/aflightentry.h
    src/classes/apilotentry.h
    src/classes/arunguard.h
    src/classes/asettings.h
    src/classes/atailentry.h
    src/database/adatabase.h
    src/database/adatabasesetup.h
    src/database/declarations.h
    src/functions/acalc.h
    src/functions/adatetime.h
    src/functions/areadcsv.h
    src/functions/astat.h
    src/functions/atime.h
    src/gui/dialogues/firstrundialog.h
    src/gui/dialogues/newflightdialog.h
    src/gui/dialogues/newpilotdialog.h
    src/gui/dialogues/newtaildialog.h
    src/gui/widgets/aircraftwidget.h
    src/gui/widgets/debugwidget.h
    src/gui/widgets/homewidget.h
    src/gui/widgets/logbookwidget.h
    src/gui/widgets/pilotswidget.h
    src/gui/widgets/settingswidget.h
    src/gui/widgets/totalswidget.h
    src/oplconstants.h
    src/testing/abenchmark.h
    src/testing/adebug.h
    src/testing/atimer.h
    
    mainwindow.ui
    src/gui/dialogues/firstrundialog.ui
    src/gui/dialogues/newflight.ui
    src/gui/dialogues/newpilot.ui
    src/gui/dialogues/newtail.ui
    src/gui/widgets/aircraftwidget.ui
    src/gui/widgets/debugwidget.ui
    src/gui/widgets/homewidget.ui
    src/gui/widgets/logbookwidget.ui
    src/gui/widgets/pilotswidget.ui
    src/gui/widgets/settingswidget.ui
    src/gui/widgets/totalswidget.ui
    assets/icons.qrc
    assets/themes/breeze.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(openPilotLog
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(openPilotLog SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(openPilotLog
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(openPilotLog PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Network)

install(TARGETS openPilotLog DESTINATION bin)
