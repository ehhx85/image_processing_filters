QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = "Imaging Filters for OSX"
TEMPLATE = app

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12

INCLUDEPATH += /opt/local/include/

LIBS += -L/opt/local/lib \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_highgui \
    -lopencv_ml \
    -lopencv_video \
    -lopencv_features2d \
    -lopencv_calib3d \
    -lopencv_objdetect \
    -lopencv_flann \
    -lopencv_imgcodecs

include(res/res.pri)
include(gui/gui.pri)
include(imaging/imaging.pri)

SOURCES += main.cpp
