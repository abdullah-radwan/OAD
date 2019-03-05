#-------------------------------------------------
#
# Project created by QtCreator 2018-09-06T07:58:27
#
#-------------------------------------------------

QT += core gui widgets

TEMPLATE = app
TARGET = ../OAD

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

LIBS += -lz
LIBS += -lquazip

INCLUDEPATH += tmp/moc/release_shared

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    configeditor.cpp \
    addonsops.cpp \
    settingswindow.cpp \
    adddbdialog.cpp \
    installdialog.cpp \
    addigndialog.cpp \
    settingsops.cpp \
    opsdialog.cpp \
    zipthread.cpp

HEADERS += \
    mainwindow.h \
    configeditor.h \
    addonsops.h \
    settingswindow.h \
    adddbdialog.h \
    installdialog.h \
    addigndialog.h \
    settingsops.h \
    opsdialog.h \
    zipthread.h

FORMS += \
    mainwindow.ui \
    settingswindow.ui \
    adddbdialog.ui \
    installdialog.ui \
    addigndialog.ui \
    opsdialog.ui

RESOURCES += \
    res.qrc

RC_ICONS = ../res/satellite.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
