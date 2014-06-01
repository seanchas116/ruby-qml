#include "ext_metaobject.h"
#include "ext_objectpointer.h"
#include "ext_pluginloader.h"
#include "ext_gcmarker.h"
#include "signalforwarder.h"
#include <QtCore/QSet>

using namespace RubyQml;

namespace {

void defineTestUtil()
{
    rb_define_module_under(rb_path2class("QML"), "TestUtil");

    auto echo_conversion = [](VALUE klass, VALUE value) {
        Q_UNUSED(klass);
        VALUE ret;
        unprotect([&] {
            ret = echoConversion(value);
        });
        return ret;
    };

    rb_define_module_function(rb_path2class("QML::TestUtil"), "echo_conversion",
                              (VALUE (*)(...))(VALUE (*)(VALUE, VALUE))echo_conversion, 1);
}

void defineMetaTypes()
{
    unprotect([&] {
        qRegisterMetaType<const QMetaObject *>();
    });
}

void defineClasses()
{
    unprotect([&] {
        Ext::MetaObject::defineClass();
        Ext::ObjectPointer::defineClass();
        Ext::PluginLoader::defineClass();
        Ext::GCMarker::defineClass();
    });
}

void setupGlobalGCMarking()
{
    auto marker = Ext::GCMarker::newAsRuby();
    Ext::GCMarker::getPointer(marker)->setMarkFunction([]{
        for (auto value : globalMarkValues()) {
            rb_gc_mark(value);
        }
    });
    rb_gc_register_mark_object(marker);
}

void cleanup()
{
    SignalForwarder::deleteAll();
}

void setupEndProc()
{
    rb_set_end_proc([](VALUE) { unprotect(cleanup); }, Qnil);
}

}

extern "C"
void Init_qml()
{
    defineMetaTypes();

    rb_require("qml/errors");
    rb_require("qml/error_converter");

    defineTestUtil();
    defineClasses();
    setupGlobalGCMarking();
    setupEndProc();
}
