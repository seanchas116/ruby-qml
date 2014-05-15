#include "metaobject.h"
#include "objectpointer.h"
#include "pluginloader.h"
#include "gcmarker.h"
#include <QtCore/QSet>

using namespace RubyQml;

namespace {

void defineTestUtil()
{
    rb_define_module_under(rb_path2class("QML"), "TestUtil");

    auto echo_conversion = [](VALUE klass, VALUE value) {
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
        MetaObject::defineClass();
        ObjectPointer::defineClass();
        PluginLoader::defineClass();
        GCMarker::defineClass();
    });
}

void setupGlobalGCMarking()
{
    auto marker = GCMarker::newAsRuby();
    GCMarker::getPointer(marker)->setMarkFunction([]{
        for (auto value : globalMarkValues()) {
            rb_gc_mark(value);
        }
    });
    rb_gc_register_mark_object(marker);
}

void setupExitHandlerObject()
{
    rb_set_end_proc([](VALUE) {
        unprotect([] {
            delete exitHandlerObject();
        });
    }, Qnil);
}

}

extern "C"
void Init_qml()
{
    defineMetaTypes();

    rb_require("qml/errors");

    defineTestUtil();
    defineClasses();
    setupGlobalGCMarking();
    setupExitHandlerObject();
}
