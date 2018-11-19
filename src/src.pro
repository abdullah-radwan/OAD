#-------------------------------------------------
#
# Project created by QtCreator 2018-09-06T07:58:27
#
#-------------------------------------------------

QT += core gui widgets

TEMPLATE = app
TARGET = ../OAD

INCLUDEPATH += D:/Qt/zlib-1.2.11
LIBS += -LD:/Qt/zlib-1.2.11 -lz
INCLUDEPATH += D:/Qt/quazip-0.7.3/quazip
LIBS += -LD:/Qt/quazip-0.7.3/quazip/release -lquazip

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    configeditor.cpp \
    addonsops.cpp \
    settingswindow.cpp \
    adddbdialog.cpp \
    installdialog.cpp \
    addigndialog.cpp \
    settingsops.cpp

HEADERS += \
        mainwindow.h \
    configeditor.h \
    addonsops.h \
    settingswindow.h \
    adddbdialog.h \
    installdialog.h \
    addigndialog.h \
    settingsops.h

FORMS += \
        mainwindow.ui \
    settingswindow.ui \
    adddbdialog.ui \
    installdialog.ui \
    addigndialog.ui

RESOURCES += \
    res.qrc

RC_ICONS = res/icon/satellite.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
