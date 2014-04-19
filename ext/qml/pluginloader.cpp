#include "pluginloader.h"
#include "objectbase.h"

namespace RubyQml {

PluginLoader::PluginLoader() :
    mPluginLoader(new QPluginLoader())
{
}

PluginLoader::~PluginLoader()
{
}

VALUE PluginLoader::initialize(VALUE path)
{
    mPluginLoader->setFileName(fromRuby<QString>(path));
    return self();
}

VALUE PluginLoader::load()
{
    auto ok = mPluginLoader->load();
    if (!ok) {
        fail("QML::PluginError", mPluginLoader->errorString());
    }
    return self();
}

VALUE PluginLoader::instance()
{
    return ObjectBase::newAsRuby(mPluginLoader->instance(), false);
}

PluginLoader::Definition PluginLoader::createDefinition()
{
    return Definition("QML", "PluginLoader")
        .defineMethod<METHOD_TYPE_NAME(&PluginLoader::initialize)>("initialize", MethodAccess::Private)
        .defineMethod<METHOD_TYPE_NAME(&PluginLoader::load)>("load")
        .defineMethod<METHOD_TYPE_NAME(&PluginLoader::instance)>("instance");
}

} // namespace RubyQml
