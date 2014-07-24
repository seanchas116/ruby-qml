#include "ext_pluginloader.h"
#include "ext_pointer.h"
#include "rubyclass.h"
#include <QtCore/QPluginLoader>
#include <QtCore/QSet>

namespace RubyQml {

Ext_PluginLoader::Ext_PluginLoader(RubyValue self) :
    self(self),
    mPluginLoader(new QPluginLoader())
{
}

Ext_PluginLoader::~Ext_PluginLoader()
{
}

RubyValue Ext_PluginLoader::initialize(RubyValue path)
{
    mPluginLoader->setFileName(path.to<QString>());
    return self;
}

RubyValue Ext_PluginLoader::load()
{
    auto ok = mPluginLoader->load();
    if (!ok) {
        fail("QML::PluginError", mPluginLoader->errorString());
    }
    return self;
}

RubyValue Ext_PluginLoader::instance()
{
    self.send("load");
    auto instance = mPluginLoader->instance();
    if (instance) {
        return RubyValue::from(instance);
    } else {
        return Qnil;
    }
}

void Ext_PluginLoader::defineClass()
{
    WrapperRubyClass<Ext_PluginLoader> klass(RubyModule::fromPath("QML"), "PluginLoader");
    klass.defineMethod(MethodAccess::Private, "initialize", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_PluginLoader::initialize));
    klass.defineMethod(MethodAccess::Public, "load", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_PluginLoader::load));
    klass.defineMethod(MethodAccess::Public, "instance", RUBYQML_MEMBER_FUNCTION_INFO(&Ext_PluginLoader::instance));
}

} // namespace RubyQml
