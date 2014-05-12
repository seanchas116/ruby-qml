#include "extension.h"
#include "metaobject.h"
#include "objectpointer.h"
#include "pluginloader.h"

namespace RubyQml {

namespace {

Extension *extension_ = nullptr;

}

Extension::Extension(QObject *parent) :
    QObject(parent)
{
    protect([&] {
        rb_require("qml/errors");

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
    });

    MetaObject::defineClass();
    ObjectPointer::defineClass();
    PluginLoader::defineClass();
}

Extension *Extension::instance()
{
    return extension_;
}

} // namespace RubyQml

using namespace RubyQml;

extern "C"
void Init_qml()
{
    unprotect([&] {
        extension_ = new Extension();
    });

    rb_set_end_proc([](VALUE) {
        unprotect([] {
            delete extension_;
        });
    }, Qnil);
}
