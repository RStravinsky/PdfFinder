#-------------------------------------------------
#
# Project created by QtCreator 2015-11-24T07:04:36
#
#-------------------------------------------------

QT += core gui xlsx multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PdfFinder
TEMPLATE = app

CONFIG += c++11
CONFIG += debug


SOURCES += main.cpp\
     mainwindow.cpp \
     settingsdialog.cpp \
     finder.cpp \
    mergedialog.cpp \
    helpdialog.cpp

HEADERS  += mainwindow.h \
      settingsdialog.h \
      finder.h \
    mergedialog.h \
    helpdialog.h

FORMS    += mainwindow.ui \
    settingsdialog.ui \
    mergedialog.ui \
    helpdialog.ui

RESOURCES += \
    resources.qrc

RC_ICONS = images\icon.ico
