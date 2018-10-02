QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = "Imaging Basics for Windows 10"
TEMPLATE = app

INCLUDEPATH += "C:\OpenCV-3.2.0\opencv\build\include"

LIBPATH += "C:\OpenCV-3.2.0\opencv\sources\build\lib\Release"

LIBS += -lopencv_core320 \
    -lopencv_imgproc320 \
    -lopencv_highgui320 \
    -lopencv_ml320 \
    -lopencv_video320 \
    -lopencv_features2d320 \
    -lopencv_calib3d320 \
    -lopencv_objdetect320 \
    -lopencv_flann320 \
    -lopencv_imgcodecs320

include(res/res.pri)
include(gui/gui.pri)
include(imaging/imaging.pri)

SOURCES += main.cpp
