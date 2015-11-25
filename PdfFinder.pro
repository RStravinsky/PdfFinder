#-------------------------------------------------
#
# Project created by QtCreator 2015-11-24T07:04:36
#
#-------------------------------------------------

QT       += core gui xlsx

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PdfFinder
TEMPLATE = app

CONFIG += c++11
CONFIG += debug


SOURCES += main.cpp\
        mainwindow.cpp \
    finder.cpp

HEADERS  += mainwindow.h \
    finder.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
