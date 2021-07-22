QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 static

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    fdsettings.cpp \
    gamewindow.cpp \
    host_dialog.cpp \
    join_dialog.cpp \
    launcher.cpp \
    main.cpp \
    gameselectwindow.cpp \
    multiplayer_dialog.cpp \
    resolution.cpp \
    settings_dialog.cpp \
    infodialog.cpp

HEADERS += \
    fdsettings.h \
    gameselectwindow.h \
    gamewindow.h \
    host_dialog.h \
    ini.h \
    join_dialog.h \
    launcher.h \
    multiplayer_dialog.h \
    resolution.h \
    settings_dialog.h \
    infodialog.h

FORMS += \
    gameselectwindow.ui \
    gamewindow.ui \
    host_dialog.ui \
    join_dialog.ui \
    multiplayer_dialog.ui \
    settings_dialog.ui \
    infodialog.ui

TRANSLATIONS += \
    FDLauncher_pt.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

RC_FILE = app.rc
