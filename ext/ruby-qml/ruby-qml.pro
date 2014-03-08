TARGET = ruby-qml
TEMPLATE = lib
QT += qml quick testlib
CONFIG += c++11

SOURCES += \
    variant.cpp \
    testobject.cpp \
    object.cpp \
    metaobject.cpp

#target.target = install
#target.commands = $(COPY_FILE) $(PWD)/$(TARGET) $(DESTDIR)
#QMAKE_EXTRA_TARGETS += target

HEADERS +=
