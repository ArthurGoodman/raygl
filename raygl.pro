QT += core gui widgets opengl

CONFIG += c++11

TARGET = raygl
TEMPLATE = app

SOURCES += main.cpp \
    glwidget.cpp \
    offscreen.cpp

HEADERS  += \
    glwidget.h \
    offscreen.h

RESOURCES += \
    resources.qrc
