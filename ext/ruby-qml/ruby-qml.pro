TARGET = ruby-qml
TEMPLATE = lib
QT += qml quick testlib
CONFIG += c++11

SOURCES += \
	clib/variant.cpp \
	clib/object.cpp \
	clib/metaobject.cpp \
	testobject.cpp

#target.target = install
#target.commands = $(COPY_FILE) $(PWD)/$(TARGET) $(DESTDIR)
#QMAKE_EXTRA_TARGETS += target

HEADERS +=
