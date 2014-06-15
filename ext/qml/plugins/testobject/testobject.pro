TEMPLATE = lib
CONFIG += plugin c++11
QT += qml

TARGET = rubyqml-testobject
INCLUDEPATH += .

# Input
SOURCES += testobject.cpp \
    testobjectplugin.cpp \
    testobjectsubclass.cpp \
    ownershiptest.cpp

HEADERS += \
    testobjectplugin.h \
    testobject.h \
    testobjectsubclass.h \
    ownershiptest.h
