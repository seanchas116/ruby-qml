#-------------------------------------------------
#
# Project created by QtCreator 2014-05-12T21:03:35
#
#-------------------------------------------------

QT       += core gui qml quick

TARGET = rubyqml-core
TEMPLATE = lib
CONFIG += plugin c++11

SOURCES += coreplugin.cpp \
    applicationwrapper.cpp \
    enginewrapper.cpp \
    componentwrapper.cpp \
    contextwrapper.cpp

HEADERS += coreplugin.h \
    applicationwrapper.h \
    enginewrapper.h \
    componentwrapper.h \
    contextwrapper.h
