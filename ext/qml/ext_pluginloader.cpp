#include "ext_pluginloader.h"
#include "ext_qtobjectpointer.h"
#include "rubyclass.h"
#include <QtCore/QPluginLoader>
#include <QtCore/QSet>

namespace RubyQml {
namespace Ext {

PluginLoader::PluginLoader(RubyValue self) :
    self(self),
    mPluginLoader(new QPluginLoader())
{
}

PluginLoader::~PluginLoader()
{
}

RubyValue PluginLoader::initialize(RubyValue path)
{
    mPluginLoader->setFileName(path.to<QString>());
    return self;
}

RubyValue PluginLoader::load()
{
    auto ok = mPluginLoader->load();
    if (!ok) {
        fail("QML::PluginError", mPluginLoader->errorString());
    }
    return self;
}

RubyValue PluginLoader::instance()
{
    self.send("load");
    auto instance = mPluginLoader->instance();
    if (instance) {
        return RubyValue::from(instance);
    } else {
        return Qnil;
    }
}

void PluginLoader::defineClass()
{
    WrapperRubyClass<PluginLoader> klass("QML", "PluginLoader");
    klass.defineMethod(MethodAccess::Private, "initialize", RUBYQML_MEMBER_FUNCTION_INFO(&PluginLoader::initialize));
    klass.defineMethod(MethodAccess::Public, "load", RUBYQML_MEMBER_FUNCTION_INFO(&PluginLoader::load));
    klass.defineMethod(MethodAccess::Public, "instance", RUBYQML_MEMBER_FUNCTION_INFO(&PluginLoader::instance));
}

} // namespace Ext
} // namespace RubyQml
