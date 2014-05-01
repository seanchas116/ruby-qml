#include "metaobject.h"
#include "objectbase.h"
#include "pluginloader.h"

using namespace RubyQml;

extern "C"
void Init_qml()
{
    rb_require("qml/errors");
    MetaObject::defineClass();
    ObjectBase::defineClass();
    PluginLoader::defineClass();
    rb_define_module_under(rb_path2class("QML"), "TestUtil");

    auto echo_conversion = [](VALUE klass, VALUE value) {
        //return unprotect([&] {
            return echoConversion(value);
        //});
    };

    rb_define_module_function(rb_path2class("QML::TestUtil"), "echo_conversion",
                              (VALUE (*)(...))(VALUE (*)(VALUE, VALUE))echo_conversion, 1);
}
