#-------------------------------------------------
#
# Project created by QtCreator 2015-11-28T20:51:25
#
#-------------------------------------------------

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = FloatSat_GroundStation
TEMPLATE = app


SOURCES += main.cpp\
        groundstation.cpp \
    satellitelink.cpp \
    payload.cpp \
    imageprocessor.cpp \
    compass.cpp

HEADERS  += groundstation.h \
    satellitelink.h \
    payload.h \
    basics.h \
    imageprocessor.h \
    compass.h

unix:SOURCES += qcustomplot.cpp
unix:HEADERS += qcustomplot.h

FORMS    += groundstation.ui

CONFIG += console c++11



unix:INCLUDEPATH += /usr/local/include
unix:LIBS += -L/usr/local/lib \
     -lopencv_core \
     -lopencv_imgproc \
     -lopencv_highgui \
     -lopencv_features2d\

win32:INCLUDEPATH += C:/opencv/modules/core/include
win32:INCLUDEPATH += C:/opencv/modules/imgproc/include
win32:INCLUDEPATH += C:/opencv/modules/highgui/include
win32:INCLUDEPATH += C:/opencv/modules/imgcodecs/include
win32:INCLUDEPATH += C:/opencv/modules/videoio/include
win32:INCLUDEPATH += C:/qcustomplot-source

win32:CONFIG(debug, release|debug) {
  QCPLIB = qcustomplotd1
} else {
  QCPLIB = qcustomplot1
}
win32:LIBS += -LC:/opencvbuild/lib \
        -LC:/opencvbuild/bin \
        -LC:/qcustomplot-source/qcustomplot-sharedlib/sharedlib-compilation/release \
        -LC:/qcustomplot-source/qcustomplot-sharedlib/sharedlib-compilation/debug \
        -lopencv_core310 \
        -lopencv_imgproc310 \
        -lopencv_highgui310 \
        -lopencv_features2d310 \
        -l$$QCPLIB
