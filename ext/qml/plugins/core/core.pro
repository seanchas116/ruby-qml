#-------------------------------------------------
#
# Project created by QtCreator 2014-05-12T21:03:35
#
#-------------------------------------------------

QT       += core gui qml quick widgets

TARGET = rubyqml-core
TEMPLATE = lib
CONFIG += plugin c++11

SOURCES += coreplugin.cpp \
    qmlexception.cpp \
    applicationextension.cpp \
    componentextension.cpp \
    contextextension.cpp \
    engineextension.cpp

HEADERS += coreplugin.h \
    qmlexception.h \
    applicationextension.h \
    componentextension.h \
    contextextension.h \
    engineextension.h
