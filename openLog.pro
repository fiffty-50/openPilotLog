QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = openLog

RESOURCES = themes/breeze.qrc \
    themes/icons/icons.qrc

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES *= QT_USE_QSTRINGBUILDER # more efficient use of string concatenation

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calc.cpp \
    dbaircraft.cpp \
    dbairport.cpp \
    dbflight.cpp \
    dbpilots.cpp \
    dbsettings.cpp \
    dbsetup.cpp \
    dbstat.cpp \
    easaview.cpp \
    editflight.cpp \
    homewidget.cpp \
    logbookwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    newacft.cpp \
    newflight.cpp \
    settingswidget.cpp \
    showaircraftlist.cpp \
    strictregularexpressionvalidator.cpp \

HEADERS += \
    calc.h \
    dbaircraft.h \
    dbairport.h \
    dbflight.h \
    dbpilots.h \
    dbsettings.h \
    dbsetup.h \
    dbstat.h \
    easaview.h \
    editflight.h \
    homewidget.h \
    logbookwidget.h \
    mainwindow.h \
    newacft.h \
    newflight.h \
    settingswidget.h \
    showaircraftlist.h \
    strictregularexpressionvalidator.h \

FORMS += \
    easaview.ui \
    editflight.ui \
    homewidget.ui \
    logbookwidget.ui \
    mainwindow.ui \
    newacft.ui \
    newflight.ui \
    settingswidget.ui \
    showaircraftlist.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Scratchpad
