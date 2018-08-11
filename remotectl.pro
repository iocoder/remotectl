#-------------------------------------------------
#
# Project created by QtCreator 2018-08-11T04:04:14
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = remotectl
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        testwindow.cpp \
    connectionmanager.cpp \
    loginwindow.cpp \
    mainwindow.cpp \
    settingsmanager.cpp

HEADERS += \
        testwindow.h \
    connectionmanager.h \
    loginwindow.h \
    mainwindow.h \
    settingsmanager.h \
    version.h

FORMS += \
        testwindow.ui \
    loginwindow.ui \
    mainwindow.ui

LIBS=-lssh2 -lqtermwidget5

RESOURCES = resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    login \
    editor \
    top \
    remotectl.png
