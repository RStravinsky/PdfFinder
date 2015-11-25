#-------------------------------------------------
#
# Project created by QtCreator 2015-11-24T07:04:36
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PdfFinder
TEMPLATE = app

CONFIG += c++11
CONFIG += debug


SOURCES += main.cpp\
        mainwindow.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    settingsdialog.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    resources.qrc
