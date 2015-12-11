#-------------------------------------------------
#
# Project created by QtCreator 2015-11-28T20:51:25
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FloatSat_GroundStation
TEMPLATE = app


SOURCES += main.cpp\
        groundstation.cpp \
    satellitelink.cpp

HEADERS  += groundstation.h \
    satellitelink.h

FORMS    += groundstation.ui

CONFIG += console
