#include "metaobject.h"
#include "objectbase.h"
#include "pluginloader.h"

extern "C"
void Init_qml()
{
    rb_require("qml/errors");
    RubyQml::MetaObject::defineClass();
    RubyQml::ObjectBase::defineClass();
    RubyQml::PluginLoader::defineClass();
}
