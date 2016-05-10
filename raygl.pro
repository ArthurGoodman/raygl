QT += core gui widgets opengl

CONFIG += c++11

TARGET = raygl
TEMPLATE = app

SOURCES += main.cpp \
    widget.cpp \
    renderer.cpp \
    console.cpp \
    consoleparser.cpp \
    shaderprogram.cpp

HEADERS  += \
    widget.h \
    renderer.h \
    console.h \
    consoleparser.h \
    shaderprogram.h

RESOURCES += \
    resources.qrc
