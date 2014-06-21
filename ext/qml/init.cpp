#include "ext_metaobject.h"
#include "ext_qtobjectpointer.h"
#include "ext_pluginloader.h"
#include "ext_gcmarker.h"
#include "ext_accesssupport.h"
#include "signalforwarder.h"
#include "valuereference.h"
#include "objectgc.h"
#include <QtCore/QSet>

using namespace RubyQml;

namespace {

RubyValue echoConversion(RubyValue mod, RubyValue value)
{
    Q_UNUSED(mod);
    auto variant = RubyValue(value).to<QVariant>();
    return RubyValue::from(variant);
}

void defineTestUtil()
{
    RubyModule testUtil("QML", "TestUtil");
    testUtil.toValue().defineSingletonMethod("echo_conversion", RUBYQML_FUNCTION_INFO(&echoConversion));
}

void defineMetaTypes()
{
    qRegisterMetaType<const QMetaObject *>();
}

void defineClasses()
{
    Ext::MetaObject::defineClass();
    Ext::QtObjectPointer::defineClass();
    Ext::PluginLoader::defineClass();
    Ext::GCMarker::defineClass();
    Ext::AccessSupport::defineClass();
}

void setupGlobalGCMarking()
{
    auto marker = Ext::GCMarker::fromMarkFunction([] {
        ValueReference::markAllReferences();
        ObjectGC::instance()->markNonOwnedObjects();
    });
    rb_gc_register_mark_object(marker);
}

void cleanup()
{
    SignalForwarder::deleteAll();
    ObjectGC::cleanUp();
}

void setupEndProc()
{
    rb_set_end_proc([](VALUE) { unprotect(cleanup); }, Qnil);
}

}

extern "C"
void Init_qml()
{
    rb_require("qml/errors");
    rb_require("qml/error_converter");

    protect([&] {
        defineMetaTypes();
        defineTestUtil();
        defineClasses();
        setupGlobalGCMarking();
        setupEndProc();
    });
}
