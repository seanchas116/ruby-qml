#-------------------------------------------------
#
# Project created by QtCreator 2014-05-12T21:03:35
#
#-------------------------------------------------

QT       += core gui

TARGET = core
TEMPLATE = lib
CONFIG += plugin

DESTDIR = $$[QT_INSTALL_PLUGINS]/generic

SOURCES += coreplugin.cpp

HEADERS += coreplugin.h
OTHER_FILES +=

unix {
    target.path = /usr/lib
    INSTALLS += target
}
