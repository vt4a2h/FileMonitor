#-------------------------------------------------
#
# Project created by QtCreator 2014-06-09T19:22:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileMonitor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        filesystemmonitor.cpp \
        windowsfilesystemmonitor.cpp \
        constants.cpp

HEADERS += mainwindow.h \
        filesystemmonitor.h \
        windowsfilesystemmonitor.h

FORMS    += mainwindow.ui

CONFIG += c++11

RESOURCES += \
    main.qrc

win32:RC_FILE = main.rc

Release:DESTDIR = release
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui
