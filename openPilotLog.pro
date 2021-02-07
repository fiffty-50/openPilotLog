QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++1z

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES *= QT_USE_QSTRINGBUILDER

# [G]: need to fix this. There must be a src/* command or smth
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/classes/astyle.cpp \
    src/classes/astandardpaths.cpp \
    src/classes/aaircraftentry.cpp \
    src/classes/adownload.cpp \
    src/classes/aentry.cpp \
    src/classes/aflightentry.cpp \
    src/classes/apilotentry.cpp \
    src/classes/arunguard.cpp \
    src/classes/asettings.cpp \
    src/classes/atailentry.cpp \
    src/database/adatabase.cpp \
    src/database/adatabasesetup.cpp \
    src/functions/acalc.cpp \
    src/functions/areadcsv.cpp \
    src/functions/astat.cpp \
    src/gui/dialogues/firstrundialog.cpp \
    src/gui/dialogues/newflightdialog.cpp \
    src/gui/dialogues/newpilotdialog.cpp \
    src/gui/dialogues/newtaildialog.cpp \
    src/gui/widgets/aircraftwidget.cpp \
    src/gui/widgets/debugwidget.cpp \
    src/gui/widgets/homewidget.cpp \
    src/gui/widgets/logbookwidget.cpp \
    src/gui/widgets/pilotswidget.cpp \
    src/gui/widgets/settingswidget.cpp \
    src/testing/abenchmark.cpp \
    src/testing/atimer.cpp

HEADERS += \
    mainwindow.h \
    src/classes/astyle.h \
    src/classes/astandardpaths.h \
    src/classes/aaircraftentry.h \
    src/classes/adownload.h \
    src/classes/aentry.h \
    src/classes/aflightentry.h \
    src/classes/apilotentry.h \
    src/classes/arunguard.h \
    src/classes/asettings.h \
    src/classes/atailentry.h \
    src/database/adatabase.h \
    src/database/adatabasesetup.h \
    src/database/adatabasetypes.h \
    src/functions/acalc.h \
    src/functions/adatetime.h \
    src/functions/areadcsv.h \
    src/functions/astat.h \
    src/functions/atime.h \
    src/gui/dialogues/firstrundialog.h \
    src/gui/dialogues/newflightdialog.h \
    src/gui/dialogues/newpilotdialog.h \
    src/gui/dialogues/newtaildialog.h \
    src/gui/widgets/aircraftwidget.h \
    src/gui/widgets/debugwidget.h \
    src/gui/widgets/homewidget.h \
    src/gui/widgets/logbookwidget.h \
    src/gui/widgets/pilotswidget.h \
    src/gui/widgets/settingswidget.h \
    src/oplconstants.h \
    src/testing/abenchmark.h \
    src/testing/adebug.h \
    src/testing/atimer.h

FORMS += \
    mainwindow.ui \
    src/gui/dialogues/firstrundialog.ui \
    src/gui/dialogues/newflight.ui \
    src/gui/dialogues/newpilot.ui \
    src/gui/dialogues/newtail.ui \
    src/gui/widgets/aircraftwidget.ui \
    src/gui/widgets/debugwidget.ui \
    src/gui/widgets/homewidget.ui \
    src/gui/widgets/logbookwidget.ui \
    src/gui/widgets/pilotswidget.ui \
    src/gui/widgets/settingswidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



RESOURCES += \
    assets/icons.qrc \
    assets/templates.qrc \
    assets/themes/stylesheets/breeze/breeze.qrc \
    assets/themes/stylesheets/qdarkstyle/qdarkstyle.qrc

DISTFILES += \
    assets/themes/dark.qss \
    assets/themes/dark/branch_closed-on.svg \
    assets/themes/dark/branch_closed.svg \
    assets/themes/dark/branch_open-on.svg \
    assets/themes/dark/branch_open.svg \
    assets/themes/dark/checkbox_checked.svg \
    assets/themes/dark/checkbox_checked_disabled.svg \
    assets/themes/dark/checkbox_indeterminate.svg \
    assets/themes/dark/checkbox_indeterminate_disabled.svg \
    assets/themes/dark/checkbox_unchecked.svg \
    assets/themes/dark/checkbox_unchecked_disabled.svg \
    assets/themes/dark/close-hover.svg \
    assets/themes/dark/close-pressed.svg \
    assets/themes/dark/close.svg \
    assets/themes/dark/down_arrow-hover.svg \
    assets/themes/dark/down_arrow.svg \
    assets/themes/dark/down_arrow_disabled.svg \
    assets/themes/dark/hmovetoolbar.svg \
    assets/themes/dark/hsepartoolbar.svg \
    assets/themes/dark/left_arrow.svg \
    assets/themes/dark/left_arrow_disabled.svg \
    assets/themes/dark/radio_checked.svg \
    assets/themes/dark/radio_checked_disabled.svg \
    assets/themes/dark/radio_unchecked.svg \
    assets/themes/dark/radio_unchecked_disabled.svg \
    assets/themes/dark/right_arrow.svg \
    assets/themes/dark/right_arrow_disabled.svg \
    assets/themes/dark/sizegrip.svg \
    assets/themes/dark/spinup_disabled.svg \
    assets/themes/dark/stylesheet-branch-end-closed.svg \
    assets/themes/dark/stylesheet-branch-end-open.svg \
    assets/themes/dark/stylesheet-branch-end.svg \
    assets/themes/dark/stylesheet-branch-more.svg \
    assets/themes/dark/stylesheet-vline.svg \
    assets/themes/dark/transparent.svg \
    assets/themes/dark/undock-hover.svg \
    assets/themes/dark/undock.svg \
    assets/themes/dark/up_arrow-hover.svg \
    assets/themes/dark/up_arrow.svg \
    assets/themes/dark/up_arrow_disabled.svg \
    assets/themes/dark/vmovetoolbar.svg \
    assets/themes/dark/vsepartoolbars.svg \
    assets/themes/dark_original.qss \
    assets/themes/light.qss \
    assets/themes/light/branch_closed-on.svg \
    assets/themes/light/branch_closed.svg \
    assets/themes/light/branch_open-on.svg \
    assets/themes/light/branch_open.svg \
    assets/themes/light/checkbox_checked-hover.svg \
    assets/themes/light/checkbox_checked.svg \
    assets/themes/light/checkbox_checked_disabled.svg \
    assets/themes/light/checkbox_indeterminate-hover.svg \
    assets/themes/light/checkbox_indeterminate.svg \
    assets/themes/light/checkbox_indeterminate_disabled.svg \
    assets/themes/light/checkbox_unchecked-hover.svg \
    assets/themes/light/checkbox_unchecked_disabled.svg \
    assets/themes/light/close-hover.svg \
    assets/themes/light/close-pressed.svg \
    assets/themes/light/close.svg \
    assets/themes/light/down_arrow-hover.svg \
    assets/themes/light/down_arrow.svg \
    assets/themes/light/down_arrow_disabled.svg \
    assets/themes/light/hmovetoolbar.svg \
    assets/themes/light/hsepartoolbar.svg \
    assets/themes/light/left_arrow.svg \
    assets/themes/light/left_arrow_disabled.svg \
    assets/themes/light/radio_checked-hover.svg \
    assets/themes/light/radio_checked.svg \
    assets/themes/light/radio_checked_disabled.svg \
    assets/themes/light/radio_unchecked-hover.svg \
    assets/themes/light/radio_unchecked_disabled.svg \
    assets/themes/light/right_arrow.svg \
    assets/themes/light/right_arrow_disabled.svg \
    assets/themes/light/sizegrip.svg \
    assets/themes/light/spinup_disabled.svg \
    assets/themes/light/stylesheet-branch-end-closed.svg \
    assets/themes/light/stylesheet-branch-end-open.svg \
    assets/themes/light/stylesheet-branch-end.svg \
    assets/themes/light/stylesheet-branch-more.svg \
    assets/themes/light/stylesheet-vline.svg \
    assets/themes/light/transparent.svg \
    assets/themes/light/undock-hover.svg \
    assets/themes/light/undock.svg \
    assets/themes/light/up_arrow-hover.svg \
    assets/themes/light/up_arrow.svg \
    assets/themes/light/up_arrow_disabled.svg \
    assets/themes/light/vmovetoolbar.svg \
    assets/themes/light/vsepartoolbars.svg \
    assets/themes/light_original.qss \
    assets/themes/theme_license
