#include "ext_metaobject.h"
#include "ext_qtobjectpointer.h"
#include "ext_pluginloader.h"
#include "ext_gcmarker.h"
#include "ext_accesssupport.h"
#include "signalforwarder.h"
#include "valuereference.h"
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
            auto variant = RubyValue(value).to<QVariant>();
            ret = RubyValue::from(variant);
        });
        return ret;
    };

    rb_define_module_function(rb_path2class("QML::TestUtil"), "echo_conversion",
                              (VALUE(*)(...))(VALUE (*)(VALUE, VALUE))echo_conversion, 1);
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
        Ext::MetaObject::initClass();
        Ext::QtObjectPointer::initClass();
        Ext::PluginLoader::initClass();
        Ext::GCMarker::initClass();
        Ext::AccessSupport::initClass();
    });
}

void setupGlobalGCMarking()
{
    auto marker = Ext::GCMarker::fromMarkFunction(&ValueReference::markAllReferences);
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
