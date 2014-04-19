#include "metaobject.h"
#include "objectbase.h"
#include "pluginloader.h"

extern "C"
void Init_c_lib()
{
    RubyQml::MetaObject::defineClass();
    RubyQml::ObjectBase::defineClass();
    RubyQml::PluginLoader::defineClass();
}
