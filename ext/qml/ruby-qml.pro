TARGET = ruby-qml
TEMPLATE = lib
QT += qml quick testlib
CONFIG += c++11

SOURCES += \
	clib/variant.cpp \
	clib/object.cpp \
	clib/metaobject.cpp \
	testobject.cpp \
    backendmain.cpp \
    clib/metatype.cpp \
    init.cpp

macx {
  LIB_SUFFIX = "bundle"
}
unix:!macx {
  LIB_SUFFIX = "so"
}
windows {
  LIB_SUFFIX = "dll"
}

target.target = install
target.commands = $(COPY_FILE) $(PWD)/$(TARGET) ../../lib/qml/c_lib.$$LIB_SUFFIX
QMAKE_EXTRA_TARGETS += target

HEADERS += \
    util.h \
    backendmain.h
