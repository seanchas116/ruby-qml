TEMPLATE = lib
CONFIG += plugin c++11
QT += qml quick

TARGET = rubyqml-testutil
INCLUDEPATH += .

# Input
SOURCES += testobject.cpp \
    testobjectsubclass.cpp \
    ownershiptest.cpp \
    testutilplugin.cpp \
    objectlifechecker.cpp \
    imageprovidertest.cpp

HEADERS += \
    testobject.h \
    testobjectsubclass.h \
    ownershiptest.h \
    testutilplugin.h \
    objectlifechecker.h \
    imageprovidertest.h
