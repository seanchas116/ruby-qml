TARGET = ruby-qml
TEMPLATE = lib
QT += qml quick
CONFIG += c++11

SOURCES += \
    variant.cpp

#target.target = install
#target.commands = $(COPY_FILE) $(PWD)/$(TARGET) $(DESTDIR)
#QMAKE_EXTRA_TARGETS += target
