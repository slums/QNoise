#-------------------------------------------------
#
# Project created by QtCreator 2012-05-17T15:14:27
#
#-------------------------------------------------

QT       += core gui phonon network sql

TARGET = QNoise
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    clyrics.cpp \
    clastfm.cpp \
    cdatabase.cpp \
    cnetworkaccessmanager.cpp \
    clibrary.cpp

HEADERS  += mainwindow.h \
    clyrics.h \
    clastfm.h \
    cdatabase.h \
    cnetworkaccessmanager.h \
    clibrary.h

FORMS    += mainwindow.ui

RESOURCES += \
    myResources.qrc

OTHER_FILES +=
